#!/usr/bin/env python3
"""
Detailed ChaCha ID-OD Pairs Analysis
Shows specific Input Difference -> Output Difference bit patterns
"""

import numpy as np
import pandas as pd
from collections import defaultdict, Counter

def hex_to_bits8(x: str) -> np.ndarray:
    """Convert hex string to 8-bit binary array"""
    hex_str = str(x).strip()
    if len(hex_str) == 1:
        hex_str = '0' + hex_str
    
    try:
        val = int(hex_str, 16)
        s = format(val, '08b')
        return np.array([1 if ch=='1' else 0 for ch in s], dtype=np.uint8)
    except ValueError:
        return np.zeros(8, dtype=np.uint8)

def analyze_specific_id_od_pairs():
    """Analyze specific ID -> OD bit patterns"""
    
    print("=== Detailed ChaCha ID-OD Pairs Analysis ===\n")
    
    # Load dataset
    dataset = pd.read_csv('dataset.csv')
    print(f"Loaded {len(dataset):,} samples")
    
    # Use larger sample for better statistics
    sample_size = min(100000, len(dataset))
    sample_data = dataset.sample(n=sample_size, random_state=42)
    print(f"Analyzing sample of {sample_size:,} records...\n")
    
    # Track specific ID -> OD bit patterns
    id_od_patterns = defaultdict(lambda: defaultdict(int))
    id_stats = defaultdict(lambda: {'total': 0, 'od_bits': Counter()})
    
    print("Processing ID-OD patterns...")
    for idx, row in sample_data.iterrows():
        if idx % 10000 == 0:
            print(f"  Processed {idx:,} samples...")
        
        # Input difference
        input_word = row['IDWordIndex']
        input_bit = row['IDBitIndex']
        id_key = f"{input_word},{input_bit}"
        
        id_stats[id_key]['total'] += 1
        
        # Find all active output difference bits
        active_od_bits = []
        for w in range(16):
            od_hex = row[f'CipherTextDiff_{w}']
            bits = hex_to_bits8(od_hex)
            
            for b in range(8):
                if bits[b] == 1:
                    od_bit = f"{w},{b}"
                    active_od_bits.append(od_bit)
                    id_stats[id_key]['od_bits'][od_bit] += 1
        
        # Store the complete pattern for this sample
        pattern_key = "|".join(sorted(active_od_bits))
        id_od_patterns[id_key][pattern_key] += 1
    
    return id_stats, id_od_patterns

def find_best_id_od_correlations(id_stats, top_k=10):
    """Find the strongest ID -> specific OD bit correlations"""
    
    print("\nFinding strongest ID -> OD bit correlations...")
    
    correlations = []
    
    for id_key, stats in id_stats.items():
        total_samples = stats['total']
        
        # Find the most frequent output bits for this input
        for od_bit, count in stats['od_bits'].most_common(20):  # Top 20 OD bits
            probability = count / total_samples
            
            # Only consider correlations with reasonable probability
            if probability >= 0.1:  # At least 10% correlation
                correlations.append({
                    'input_diff': id_key,
                    'output_diff': od_bit,
                    'probability': probability,
                    'count': count,
                    'total_samples': total_samples
                })
    
    # Sort by probability
    correlations.sort(key=lambda x: x['probability'], reverse=True)
    
    return correlations[:top_k * 10]  # Return more for filtering

def find_best_complete_patterns(id_od_patterns, top_k=5):
    """Find the most common complete ID -> OD patterns"""
    
    print("Finding most common complete ID -> OD patterns...")
    
    pattern_analysis = []
    
    for id_key, patterns in id_od_patterns.items():
        total_samples = sum(patterns.values())
        
        # Find most common complete patterns for this input
        for pattern, count in patterns.most_common(3):  # Top 3 patterns per input
            probability = count / total_samples
            
            if probability >= 0.05 and count >= 10:  # At least 5% and 10 occurrences
                # Parse the pattern
                od_bits = pattern.split("|") if pattern else []
                
                pattern_analysis.append({
                    'input_diff': id_key,
                    'output_pattern': od_bits,
                    'probability': probability,
                    'count': count,
                    'total_samples': total_samples,
                    'pattern_size': len(od_bits)
                })
    
    # Sort by probability and pattern significance
    pattern_analysis.sort(key=lambda x: (x['probability'], x['count']), reverse=True)
    
    return pattern_analysis[:top_k * 5]

def print_detailed_results(correlations, patterns):
    """Print detailed ID-OD analysis results"""
    
    print("\n" + "="*80)
    print("BEST ID-OD PAIRS FOR CHACHA 3RD ROUND")
    print("="*80)
    
    print("\n1. STRONGEST ID -> SPECIFIC OD BIT CORRELATIONS:")
    print("-" * 70)
    print(f"{'Rank':<4} {'Input (Word,Bit)':<15} {'Output (Word,Bit)':<16} {'Probability':<12} {'Count':<8}")
    print("-" * 70)
    
    for i, corr in enumerate(correlations[:20], 1):
        input_parts = corr['input_diff'].split(',')
        output_parts = corr['output_diff'].split(',')
        
        print(f"{i:<4} {input_parts[0]},{input_parts[1]:<13} "
              f"Word {output_parts[0]}, Bit {output_parts[1]:<8} "
              f"{corr['probability']:<12.3f} {corr['count']:<8}")
    
    print("\n2. MOST PREDICTABLE COMPLETE OD PATTERNS:")
    print("-" * 80)
    
    for i, pattern in enumerate(patterns[:10], 1):
        input_parts = pattern['input_diff'].split(',')
        
        print(f"\n{i}. Input Difference: Word '{input_parts[0]}', Bit {input_parts[1]}")
        print(f"   Probability: {pattern['probability']:.3f} ({pattern['count']}/{pattern['total_samples']} samples)")
        print(f"   Output Pattern ({pattern['pattern_size']} active bits):")
        
        # Group output bits by word for better readability
        word_bits = defaultdict(list)
        for od_bit in pattern['output_pattern']:
            if od_bit:  # Skip empty strings
                word, bit = od_bit.split(',')
                word_bits[int(word)].append(int(bit))
        
        for word in sorted(word_bits.keys()):
            bits = sorted(word_bits[word])
            print(f"     Word {word:2d}: Bits {bits}")
    
    print("\n3. SUMMARY OF BEST ID-OD PAIRS:")
    print("=" * 50)
    
    # Group by input difference to show best outputs
    input_summary = defaultdict(list)
    for corr in correlations[:30]:
        input_summary[corr['input_diff']].append(corr)
    
    print("\nTop Input Differences and their best Output targets:")
    for i, (input_diff, corrs) in enumerate(list(input_summary.items())[:10], 1):
        input_parts = input_diff.split(',')
        print(f"\n{i}. Input: Word '{input_parts[0]}', Bit {input_parts[1]}")
        
        for j, corr in enumerate(corrs[:5], 1):  # Top 5 outputs for this input
            output_parts = corr['output_diff'].split(',')
            print(f"   {j}. → Word {output_parts[0]}, Bit {output_parts[1]} "
                  f"(p={corr['probability']:.3f})")

def analyze_differential_trails():
    """Analyze differential trails - sequences of differences"""
    
    print("\n4. DIFFERENTIAL TRAIL ANALYSIS:")
    print("=" * 40)
    
    # Load a smaller sample for trail analysis
    dataset = pd.read_csv('dataset.csv')
    sample_data = dataset.sample(n=10000, random_state=42)
    
    # Look for patterns in difference propagation
    trail_patterns = defaultdict(int)
    
    for _, row in sample_data.iterrows():
        input_word = row['IDWordIndex']
        input_bit = row['IDBitIndex']
        
        # Find the "path" of differences through words
        active_words = []
        for w in range(16):
            od_hex = row[f'CipherTextDiff_{w}']
            if hex_to_bits8(od_hex).sum() > 0:
                active_words.append(w)
        
        # Create a trail signature
        if len(active_words) >= 2:
            trail_key = f"{input_word},{input_bit} -> {','.join(map(str, sorted(active_words)))}"
            trail_patterns[trail_key] += 1
    
    print("Most common differential trails:")
    for i, (trail, count) in enumerate(trail_patterns.most_common(10), 1):
        percentage = (count / len(sample_data)) * 100
        print(f"{i:2d}. {trail} ({count} times, {percentage:.2f}%)")

def main():
    """Main analysis function"""
    
    # Perform detailed analysis
    id_stats, id_od_patterns = analyze_specific_id_od_pairs()
    
    # Find best correlations and patterns
    correlations = find_best_id_od_correlations(id_stats, top_k=20)
    patterns = find_best_complete_patterns(id_od_patterns, top_k=10)
    
    # Print results
    print_detailed_results(correlations, patterns)
    
    # Analyze differential trails
    analyze_differential_trails()
    
    # Save results
    print(f"\n5. SAVING RESULTS:")
    print("=" * 30)
    
    # Save correlations to CSV
    corr_df = pd.DataFrame(correlations[:50])
    corr_df.to_csv('id_od_correlations.csv', index=False)
    print("Saved: id_od_correlations.csv")
    
    # Save patterns to CSV
    pattern_df = pd.DataFrame(patterns[:20])
    pattern_df.to_csv('id_od_patterns.csv', index=False)
    print("Saved: id_od_patterns.csv")
    
    # Print key findings
    print(f"\nKEY FINDINGS:")
    print("=" * 30)
    if correlations:
        best_corr = correlations[0]
        input_parts = best_corr['input_diff'].split(',')
        output_parts = best_corr['output_diff'].split(',')
        
        print(f"🏆 Best ID-OD pair:")
        print(f"   Input:  Word '{input_parts[0]}', Bit {input_parts[1]}")
        print(f"   Output: Word {output_parts[0]}, Bit {output_parts[1]}")
        print(f"   Probability: {best_corr['probability']:.3f}")
        print(f"   Occurrences: {best_corr['count']:,}/{best_corr['total_samples']:,}")
    
    if patterns:
        best_pattern = patterns[0]
        input_parts = best_pattern['input_diff'].split(',')
        print(f"\n🎯 Most predictable pattern:")
        print(f"   Input: Word '{input_parts[0]}', Bit {input_parts[1]}")
        print(f"   Activates {best_pattern['pattern_size']} output bits")
        print(f"   Probability: {best_pattern['probability']:.3f}")

if __name__ == "__main__":
    main()