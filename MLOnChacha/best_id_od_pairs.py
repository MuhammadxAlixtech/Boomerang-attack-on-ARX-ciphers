#!/usr/bin/env python3
"""
Best ChaCha ID-OD Pairs Analysis
Shows specific Input Difference -> Output Difference correlations
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

def analyze_id_od_correlations():
    """Analyze ID -> OD correlations"""
    
    print("=== ChaCha ID-OD Pairs Analysis ===\n")
    
    # Load dataset
    dataset = pd.read_csv('dataset.csv')
    print(f"Loaded {len(dataset):,} samples")
    
    # Use a reasonable sample size
    sample_size = min(50000, len(dataset))
    sample_data = dataset.sample(n=sample_size, random_state=42)
    print(f"Analyzing {sample_size:,} samples for ID-OD correlations...\n")
    
    # Track ID -> OD bit correlations
    id_od_correlations = defaultdict(lambda: defaultdict(int))
    id_totals = defaultdict(int)
    
    print("Processing correlations...")
    for idx, row in sample_data.iterrows():
        # Input difference
        input_word = row['IDWordIndex']
        input_bit = row['IDBitIndex']
        id_key = f"{input_word},{input_bit}"
        
        id_totals[id_key] += 1
        
        # Check each output bit
        for w in range(16):
            od_hex = row[f'CipherTextDiff_{w}']
            bits = hex_to_bits8(od_hex)
            
            for b in range(8):
                if bits[b] == 1:
                    od_key = f"{w},{b}"
                    id_od_correlations[id_key][od_key] += 1
    
    # Calculate probabilities and find best correlations
    correlations = []
    
    for id_key, od_counts in id_od_correlations.items():
        total_samples = id_totals[id_key]
        
        for od_key, count in od_counts.items():
            probability = count / total_samples
            
            # Only include significant correlations
            if probability >= 0.1 and count >= 10:
                correlations.append({
                    'input_diff': id_key,
                    'output_diff': od_key,
                    'probability': probability,
                    'count': count,
                    'total_samples': total_samples
                })
    
    # Sort by probability
    correlations.sort(key=lambda x: x['probability'], reverse=True)
    
    return correlations

def analyze_word_level_patterns():
    """Analyze word-level ID -> OD patterns"""
    
    print("Analyzing word-level patterns...")
    
    # Load dataset
    dataset = pd.read_csv('dataset.csv')
    sample_data = dataset.sample(n=30000, random_state=42)
    
    # Track ID word -> OD word correlations
    word_correlations = defaultdict(lambda: defaultdict(int))
    word_totals = defaultdict(int)
    
    for _, row in sample_data.iterrows():
        input_word = row['IDWordIndex']
        input_bit = row['IDBitIndex']
        id_key = f"{input_word},{input_bit}"
        
        word_totals[id_key] += 1
        
        # Check which output words are active
        for w in range(16):
            od_hex = row[f'CipherTextDiff_{w}']
            if hex_to_bits8(od_hex).sum() > 0:  # Word has differences
                word_correlations[id_key][w] += 1
    
    # Find best word-level correlations
    word_results = []
    
    for id_key, word_counts in word_correlations.items():
        total_samples = word_totals[id_key]
        
        for word, count in word_counts.items():
            probability = count / total_samples
            
            if probability >= 0.5:  # At least 50% correlation
                word_results.append({
                    'input_diff': id_key,
                    'output_word': word,
                    'probability': probability,
                    'count': count,
                    'total_samples': total_samples
                })
    
    word_results.sort(key=lambda x: x['probability'], reverse=True)
    
    return word_results

def print_results(bit_correlations, word_correlations):
    """Print analysis results"""
    
    print("\n" + "="*80)
    print("BEST ID-OD PAIRS FOR CHACHA 3RD ROUND")
    print("="*80)
    
    print("\n1. STRONGEST ID -> SPECIFIC OD BIT CORRELATIONS:")
    print("-" * 75)
    print(f"{'Rank':<4} {'Input (Word,Bit)':<15} {'→ Output (Word,Bit)':<18} {'Probability':<12} {'Count':<10}")
    print("-" * 75)
    
    for i, corr in enumerate(bit_correlations[:25], 1):
        input_parts = corr['input_diff'].split(',')
        output_parts = corr['output_diff'].split(',')
        
        print(f"{i:<4} {input_parts[0]},{input_parts[1]:<13} "
              f"→ Word {output_parts[0]}, Bit {output_parts[1]:<8} "
              f"{corr['probability']:<12.3f} {corr['count']:<10}")
    
    print("\n2. STRONGEST ID -> OUTPUT WORD CORRELATIONS:")
    print("-" * 70)
    print(f"{'Rank':<4} {'Input (Word,Bit)':<15} {'→ Output Word':<15} {'Probability':<12} {'Count':<10}")
    print("-" * 70)
    
    for i, corr in enumerate(word_correlations[:20], 1):
        input_parts = corr['input_diff'].split(',')
        
        print(f"{i:<4} {input_parts[0]},{input_parts[1]:<13} "
              f"→ Word {corr['output_word']:<10} "
              f"{corr['probability']:<12.3f} {corr['count']:<10}")
    
    print("\n3. DETAILED ANALYSIS OF TOP 10 BIT-LEVEL CORRELATIONS:")
    print("=" * 60)
    
    for i, corr in enumerate(bit_correlations[:10], 1):
        input_parts = corr['input_diff'].split(',')
        output_parts = corr['output_diff'].split(',')
        
        print(f"\n{i}. ID: Word '{input_parts[0]}', Bit {input_parts[1]} "
              f"→ OD: Word {output_parts[0]}, Bit {output_parts[1]}")
        print(f"   • Probability: {corr['probability']:.3f}")
        print(f"   • Occurrences: {corr['count']:,} out of {corr['total_samples']:,} samples")
        print(f"   • Strength: {corr['probability']*100:.1f}% correlation")

def analyze_specific_inputs():
    """Analyze specific high-value input differences"""
    
    print("\n4. ANALYSIS OF SPECIFIC HIGH-VALUE INPUTS:")
    print("=" * 50)
    
    # Focus on a few specific inputs
    target_inputs = [('c', 0), ('d', 3), ('e', 7), ('f', 4)]
    
    dataset = pd.read_csv('dataset.csv')
    
    for input_word, input_bit in target_inputs:
        print(f"\nInput Difference: Word '{input_word}', Bit {input_bit}")
        print("-" * 40)
        
        # Filter data for this specific input
        mask = (dataset['IDWordIndex'] == input_word) & (dataset['IDBitIndex'] == input_bit)
        input_data = dataset[mask]
        
        if len(input_data) == 0:
            print("   No data found for this input")
            continue
        
        print(f"   Samples: {len(input_data):,}")
        
        # Analyze output patterns
        output_bit_counts = defaultdict(int)
        output_word_counts = defaultdict(int)
        
        for _, row in input_data.iterrows():
            for w in range(16):
                od_hex = row[f'CipherTextDiff_{w}']
                bits = hex_to_bits8(od_hex)
                
                if bits.sum() > 0:
                    output_word_counts[w] += 1
                
                for b in range(8):
                    if bits[b] == 1:
                        output_bit_counts[(w, b)] += 1
        
        # Show top output words
        print("   Top affected output words:")
        word_items = sorted(output_word_counts.items(), key=lambda x: x[1], reverse=True)
        for word, count in word_items[:5]:
            prob = count / len(input_data)
            print(f"     Word {word:2d}: {prob:.3f} ({count:,}/{len(input_data):,})")
        
        # Show top output bits
        print("   Top affected output bits:")
        bit_items = sorted(output_bit_counts.items(), key=lambda x: x[1], reverse=True)
        for (word, bit), count in bit_items[:8]:
            prob = count / len(input_data)
            print(f"     Word {word:2d}, Bit {bit}: {prob:.3f} ({count:,}/{len(input_data):,})")

def main():
    """Main analysis function"""
    
    # Perform analysis
    bit_correlations = analyze_id_od_correlations()
    word_correlations = analyze_word_level_patterns()
    
    # Print results
    print_results(bit_correlations, word_correlations)
    
    # Analyze specific inputs
    analyze_specific_inputs()
    
    # Save results
    print(f"\n5. SAVING RESULTS:")
    print("=" * 30)
    
    # Save to CSV
    if bit_correlations:
        bit_df = pd.DataFrame(bit_correlations)
        bit_df.to_csv('chacha_id_od_bit_correlations.csv', index=False)
        print("Saved: chacha_id_od_bit_correlations.csv")
    
    if word_correlations:
        word_df = pd.DataFrame(word_correlations)
        word_df.to_csv('chacha_id_od_word_correlations.csv', index=False)
        print("Saved: chacha_id_od_word_correlations.csv")
    
    # Print summary
    print(f"\nSUMMARY:")
    print("=" * 20)
    
    if bit_correlations:
        best_bit = bit_correlations[0]
        input_parts = best_bit['input_diff'].split(',')
        output_parts = best_bit['output_diff'].split(',')
        
        print(f"🏆 Best ID-OD Bit Pair:")
        print(f"   Input:  Word '{input_parts[0]}', Bit {input_parts[1]}")
        print(f"   Output: Word {output_parts[0]}, Bit {output_parts[1]}")
        print(f"   Correlation: {best_bit['probability']:.3f}")
    
    if word_correlations:
        best_word = word_correlations[0]
        input_parts = best_word['input_diff'].split(',')
        
        print(f"\n🎯 Best ID-OD Word Pair:")
        print(f"   Input:  Word '{input_parts[0]}', Bit {input_parts[1]}")
        print(f"   Output: Word {best_word['output_word']}")
        print(f"   Correlation: {best_word['probability']:.3f}")
    
    print(f"\nTotal bit-level correlations found: {len(bit_correlations)}")
    print(f"Total word-level correlations found: {len(word_correlations)}")

if __name__ == "__main__":
    main()