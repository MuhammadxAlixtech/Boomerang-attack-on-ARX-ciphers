#!/usr/bin/env python3
"""
Quick ChaCha ID-OD Analysis - Fast version for immediate results
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

def quick_analysis():
    """Quick analysis of best ID-OD pairs"""
    
    print("=== Quick ChaCha ID-OD Analysis ===\n")
    
    # Load dataset
    dataset = pd.read_csv('dataset.csv')
    print(f"Loaded {len(dataset):,} samples")
    
    # Sample analysis on subset for speed
    sample_size = min(50000, len(dataset))
    sample_data = dataset.sample(n=sample_size, random_state=42)
    print(f"Analyzing sample of {sample_size:,} records...\n")
    
    # Track ID-OD effectiveness
    id_effectiveness = defaultdict(lambda: {'active_words': [], 'hamming_weights': [], 'output_patterns': Counter()})
    
    for idx, row in sample_data.iterrows():
        id_key = (row['IDWordIndex'], row['IDBitIndex'])
        
        # Count active output words and total hamming weight
        active_words = 0
        total_hamming = 0
        active_word_list = []
        
        for w in range(16):
            od_hex = row[f'CipherTextDiff_{w}']
            bits = hex_to_bits8(od_hex)
            hamming = bits.sum()
            
            if hamming > 0:
                active_words += 1
                active_word_list.append(w)
                total_hamming += hamming
        
        # Store metrics
        id_effectiveness[id_key]['active_words'].append(active_words)
        id_effectiveness[id_key]['hamming_weights'].append(total_hamming)
        
        # Track most common output patterns
        for w in active_word_list:
            id_effectiveness[id_key]['output_patterns'][w] += 1
    
    # Calculate effectiveness scores
    results = []
    for id_key, data in id_effectiveness.items():
        input_word, input_bit = id_key
        
        avg_active_words = np.mean(data['active_words'])
        avg_hamming = np.mean(data['hamming_weights'])
        total_samples = len(data['active_words'])
        
        # Find most common output word
        if data['output_patterns']:
            most_common_word, max_count = data['output_patterns'].most_common(1)[0]
            bias_score = max_count / total_samples
        else:
            most_common_word, bias_score = -1, 0
        
        # Effectiveness = average activity * bias (higher is better for cryptanalysis)
        effectiveness = avg_active_words * bias_score
        
        results.append({
            'input_word': input_word,
            'input_bit': input_bit,
            'avg_active_words': avg_active_words,
            'avg_hamming': avg_hamming,
            'bias_score': bias_score,
            'effectiveness': effectiveness,
            'most_common_output': most_common_word,
            'samples': total_samples
        })
    
    # Sort by effectiveness
    results.sort(key=lambda x: x['effectiveness'], reverse=True)
    
    # Print top results
    print("TOP 15 BEST ID-OD PAIRS FOR 3RD ROUND:")
    print("=" * 70)
    print(f"{'Rank':<4} {'Input':<8} {'Bit':<3} {'Avg Active':<10} {'Bias':<8} {'Effectiveness':<12} {'Best OD':<8}")
    print("-" * 70)
    
    for i, result in enumerate(results[:15], 1):
        print(f"{i:<4} {result['input_word']:<8} {result['input_bit']:<3} "
              f"{result['avg_active_words']:<10.2f} {result['bias_score']:<8.3f} "
              f"{result['effectiveness']:<12.3f} Word {result['most_common_output']:<3}")
    
    print("\nDETAILED ANALYSIS OF TOP 5:")
    print("=" * 50)
    
    for i, result in enumerate(results[:5], 1):
        print(f"\n{i}. Input Difference: Word '{result['input_word']}', Bit {result['input_bit']}")
        print(f"   • Average active output words: {result['avg_active_words']:.2f}")
        print(f"   • Average Hamming weight: {result['avg_hamming']:.2f}")
        print(f"   • Output bias score: {result['bias_score']:.3f}")
        print(f"   • Effectiveness score: {result['effectiveness']:.3f}")
        print(f"   • Most affected output word: {result['most_common_output']}")
        print(f"   • Sample size: {result['samples']:,}")
    
    # Overall output word analysis
    print(f"\nOVERALL OUTPUT WORD ACTIVITY:")
    print("=" * 40)
    
    word_totals = Counter()
    for data in id_effectiveness.values():
        for word, count in data['output_patterns'].items():
            word_totals[word] += count
    
    total_activations = sum(word_totals.values())
    print("Most frequently affected output words:")
    for word, count in word_totals.most_common(10):
        percentage = (count / total_activations) * 100
        print(f"Word {word:2d}: {count:6,} activations ({percentage:5.2f}%)")
    
    print(f"\nKEY FINDINGS:")
    print("=" * 30)
    best = results[0]
    print(f"🏆 Best ID-OD pair: Input Word '{best['input_word']}', Bit {best['input_bit']}")
    print(f"📊 Effectiveness score: {best['effectiveness']:.3f}")
    print(f"🎯 Most affected output: Word {best['most_common_output']}")
    print(f"⚡ Average active words: {best['avg_active_words']:.2f}")
    print(f"🔥 Bias score: {best['bias_score']:.3f}")
    
    return results

if __name__ == "__main__":
    results = quick_analysis()