#!/usr/bin/env python3
"""
ChaCha Best ID-OD Pairs Analysis for 3rd Round

This script analyzes the dataset to find the most effective Input Difference (ID) 
to Output Difference (OD) pairs for differential cryptanalysis of ChaCha cipher.
"""

import numpy as np
import pandas as pd
from collections import defaultdict, Counter
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.preprocessing import LabelEncoder
import warnings
warnings.filterwarnings('ignore')

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

def compute_hamming_weight(hex_val: str) -> int:
    """Compute Hamming weight (number of 1s) in hex value"""
    bits = hex_to_bits8(hex_val)
    return int(bits.sum())

def analyze_differential_characteristics(dataset):
    """Analyze differential characteristics to find best ID-OD pairs"""
    
    print("=== ChaCha ID-OD Pairs Analysis ===\n")
    
    # 1. Basic statistics
    print("1. Dataset Overview:")
    print(f"   Total samples: {len(dataset):,}")
    print(f"   Input words: {sorted(dataset['IDWordIndex'].unique())}")
    print(f"   Input bits: {dataset['IDBitIndex'].min()}-{dataset['IDBitIndex'].max()}")
    
    # 2. Analyze by input difference
    id_analysis = defaultdict(lambda: {
        'total_samples': 0,
        'active_outputs': 0,
        'avg_hamming_weight': 0,
        'output_distribution': defaultdict(int),
        'best_od_words': Counter(),
        'best_od_bits': Counter()
    })
    
    print("\n2. Processing differential pairs...")
    
    for idx, row in dataset.iterrows():
        if idx % 50000 == 0:
            print(f"   Processed {idx:,} samples...")
        
        # Input difference identifier
        id_key = (row['IDWordIndex'], row['IDBitIndex'])
        
        # Analyze output differences
        total_hamming = 0
        active_words = []
        active_bits = []
        
        for w in range(16):
            od_hex = row[f'CipherTextDiff_{w}']
            hamming = compute_hamming_weight(od_hex)
            total_hamming += hamming
            
            if hamming > 0:
                active_words.append(w)
                # Find active bits in this word
                bits = hex_to_bits8(od_hex)
                for b in range(8):
                    if bits[b] == 1:
                        active_bits.append((w, b))
        
        # Update statistics
        stats = id_analysis[id_key]
        stats['total_samples'] += 1
        stats['active_outputs'] += len(active_words)
        stats['avg_hamming_weight'] += total_hamming
        
        # Track output word distribution
        for w in active_words:
            stats['output_distribution'][w] += 1
            stats['best_od_words'][w] += 1
        
        # Track output bit distribution
        for w, b in active_bits:
            stats['best_od_bits'][(w, b)] += 1
    
    # Normalize averages
    for id_key, stats in id_analysis.items():
        if stats['total_samples'] > 0:
            stats['avg_hamming_weight'] /= stats['total_samples']
            stats['active_outputs'] /= stats['total_samples']
    
    return id_analysis

def find_best_characteristics(id_analysis, top_k=10):
    """Find the best differential characteristics"""
    
    print("\n3. Finding Best Differential Characteristics...")
    
    # Metrics for ranking characteristics
    characteristics = []
    
    for id_key, stats in id_analysis.items():
        input_word, input_bit = id_key
        
        # Calculate effectiveness metrics
        avg_active_words = stats['active_outputs']
        avg_hamming = stats['avg_hamming_weight']
        total_samples = stats['total_samples']
        
        # Bias score: how concentrated the output differences are
        output_counts = list(stats['output_distribution'].values())
        if output_counts:
            max_count = max(output_counts)
            bias_score = max_count / total_samples
        else:
            bias_score = 0
        
        # Effectiveness score (higher is better for cryptanalysis)
        effectiveness = avg_active_words * bias_score
        
        characteristics.append({
            'input_word': input_word,
            'input_bit': input_bit,
            'avg_active_words': avg_active_words,
            'avg_hamming_weight': avg_hamming,
            'bias_score': bias_score,
            'effectiveness': effectiveness,
            'total_samples': total_samples,
            'best_output_words': stats['best_od_words'].most_common(5),
            'best_output_bits': stats['best_od_bits'].most_common(10)
        })
    
    # Sort by effectiveness
    characteristics.sort(key=lambda x: x['effectiveness'], reverse=True)
    
    return characteristics[:top_k]

def analyze_output_patterns(dataset):
    """Analyze output difference patterns"""
    
    print("\n4. Analyzing Output Difference Patterns...")
    
    # Word-level activity analysis
    word_activity = np.zeros(16)
    bit_activity = np.zeros((16, 8))
    
    for idx, row in dataset.iterrows():
        for w in range(16):
            od_hex = row[f'CipherTextDiff_{w}']
            bits = hex_to_bits8(od_hex)
            
            if bits.sum() > 0:
                word_activity[w] += 1
                
            for b in range(8):
                if bits[b] == 1:
                    bit_activity[w, b] += 1
    
    # Normalize by total samples
    word_activity = word_activity / len(dataset)
    bit_activity = bit_activity / len(dataset)
    
    return word_activity, bit_activity

def print_results(best_characteristics, word_activity, bit_activity):
    """Print analysis results"""
    
    print("\n" + "="*60)
    print("BEST ID-OD PAIRS FOR 3RD ROUND CHACHA")
    print("="*60)
    
    print(f"\nTop {len(best_characteristics)} Most Effective Input Differences:")
    print("-" * 80)
    print(f"{'Rank':<4} {'Input':<8} {'Bit':<3} {'Avg Active':<10} {'Bias':<8} {'Effectiveness':<12} {'Samples':<8}")
    print("-" * 80)
    
    for i, char in enumerate(best_characteristics, 1):
        print(f"{i:<4} {char['input_word']:<8} {char['input_bit']:<3} "
              f"{char['avg_active_words']:<10.2f} {char['bias_score']:<8.3f} "
              f"{char['effectiveness']:<12.3f} {char['total_samples']:<8}")
    
    print("\nDetailed Analysis of Top 5 Characteristics:")
    print("=" * 60)
    
    for i, char in enumerate(best_characteristics[:5], 1):
        print(f"\n{i}. Input Difference: Word '{char['input_word']}', Bit {char['input_bit']}")
        print(f"   - Average active output words: {char['avg_active_words']:.2f}")
        print(f"   - Average Hamming weight: {char['avg_hamming_weight']:.2f}")
        print(f"   - Bias score: {char['bias_score']:.3f}")
        print(f"   - Effectiveness: {char['effectiveness']:.3f}")
        print(f"   - Total samples: {char['total_samples']:,}")
        
        print(f"   - Most affected output words:")
        for word, count in char['best_output_words']:
            percentage = (count / char['total_samples']) * 100
            print(f"     * Word {word:2d}: {count:6,} times ({percentage:5.1f}%)")
        
        print(f"   - Most affected output bits:")
        for (word, bit), count in char['best_output_bits']:
            percentage = (count / char['total_samples']) * 100
            print(f"     * Word {word:2d}, Bit {bit}: {count:6,} times ({percentage:5.1f}%)")
    
    print("\n" + "="*60)
    print("OVERALL OUTPUT DIFFERENCE PATTERNS")
    print("="*60)
    
    print("\nMost Active Output Words (across all inputs):")
    word_rankings = [(i, activity) for i, activity in enumerate(word_activity)]
    word_rankings.sort(key=lambda x: x[1], reverse=True)
    
    for i, (word, activity) in enumerate(word_rankings[:10], 1):
        print(f"{i:2d}. Word {word:2d}: {activity:.4f} ({activity*100:.2f}% of samples)")
    
    print("\nMost Active Output Bits (top 20):")
    bit_rankings = []
    for w in range(16):
        for b in range(8):
            bit_rankings.append(((w, b), bit_activity[w, b]))
    
    bit_rankings.sort(key=lambda x: x[1], reverse=True)
    
    for i, ((word, bit), activity) in enumerate(bit_rankings[:20], 1):
        print(f"{i:2d}. Word {word:2d}, Bit {bit}: {activity:.4f} ({activity*100:.2f}% of samples)")

def create_visualizations(best_characteristics, word_activity, bit_activity):
    """Create visualizations of the analysis"""
    
    print("\n5. Creating Visualizations...")
    
    # Set up the plotting style
    plt.style.use('default')
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # 1. Top characteristics effectiveness
    top_chars = best_characteristics[:10]
    labels = [f"{c['input_word']},{c['input_bit']}" for c in top_chars]
    effectiveness = [c['effectiveness'] for c in top_chars]
    
    axes[0, 0].bar(range(len(labels)), effectiveness)
    axes[0, 0].set_xlabel('Input Difference (Word,Bit)')
    axes[0, 0].set_ylabel('Effectiveness Score')
    axes[0, 0].set_title('Top 10 Most Effective Input Differences')
    axes[0, 0].set_xticks(range(len(labels)))
    axes[0, 0].set_xticklabels(labels, rotation=45)
    
    # 2. Word activity heatmap
    axes[0, 1].bar(range(16), word_activity)
    axes[0, 1].set_xlabel('Output Word Index')
    axes[0, 1].set_ylabel('Activity Rate')
    axes[0, 1].set_title('Output Word Activity Rates')
    axes[0, 1].set_xticks(range(16))
    
    # 3. Bit activity heatmap
    im = axes[1, 0].imshow(bit_activity.T, cmap='YlOrRd', aspect='auto')
    axes[1, 0].set_xlabel('Output Word Index')
    axes[1, 0].set_ylabel('Bit Position')
    axes[1, 0].set_title('Output Bit Activity Heatmap')
    axes[1, 0].set_xticks(range(16))
    axes[1, 0].set_yticks(range(8))
    plt.colorbar(im, ax=axes[1, 0])
    
    # 4. Input difference distribution
    input_words = [c['input_word'] for c in best_characteristics[:10]]
    input_bits = [c['input_bit'] for c in best_characteristics[:10]]
    
    scatter = axes[1, 1].scatter(input_bits, [ord(w) - ord('a') for w in input_words], 
                                s=[c['effectiveness']*1000 for c in best_characteristics[:10]], 
                                alpha=0.6, c=effectiveness[:10], cmap='viridis')
    axes[1, 1].set_xlabel('Input Bit Position')
    axes[1, 1].set_ylabel('Input Word (0=a, 1=b, etc.)')
    axes[1, 1].set_title('Input Difference Distribution (size = effectiveness)')
    axes[1, 1].set_yticks(range(4))
    axes[1, 1].set_yticklabels(['c', 'd', 'e', 'f'])
    plt.colorbar(scatter, ax=axes[1, 1])
    
    plt.tight_layout()
    plt.savefig('chacha_id_od_analysis.png', dpi=300, bbox_inches='tight')
    print("   Saved visualization: chacha_id_od_analysis.png")
    
    # Show the plot
    plt.show()

def main():
    """Main analysis function"""
    
    # Load dataset
    print("Loading dataset...")
    try:
        dataset = pd.read_csv('dataset.csv')
    except FileNotFoundError:
        print("Error: dataset.csv not found!")
        return
    
    # Perform analysis
    id_analysis = analyze_differential_characteristics(dataset)
    best_characteristics = find_best_characteristics(id_analysis, top_k=20)
    word_activity, bit_activity = analyze_output_patterns(dataset)
    
    # Print results
    print_results(best_characteristics, word_activity, bit_activity)
    
    # Create visualizations
    try:
        create_visualizations(best_characteristics, word_activity, bit_activity)
    except Exception as e:
        print(f"Note: Could not create visualizations: {e}")
    
    # Save detailed results
    print("\n6. Saving Results...")
    
    # Save to CSV for further analysis
    results_df = pd.DataFrame(best_characteristics)
    results_df.to_csv('best_id_od_pairs.csv', index=False)
    print("   Saved detailed results: best_id_od_pairs.csv")
    
    # Save summary
    with open('id_od_analysis_summary.txt', 'w') as f:
        f.write("ChaCha ID-OD Pairs Analysis Summary\n")
        f.write("="*40 + "\n\n")
        
        f.write("Top 10 Most Effective Input Differences:\n")
        f.write("-" * 40 + "\n")
        
        for i, char in enumerate(best_characteristics[:10], 1):
            f.write(f"{i:2d}. Input: {char['input_word']},{char['input_bit']} | ")
            f.write(f"Effectiveness: {char['effectiveness']:.3f} | ")
            f.write(f"Avg Active Words: {char['avg_active_words']:.2f}\n")
        
        f.write(f"\nMost affected output words:\n")
        for i, activity in enumerate(word_activity):
            if activity > 0.5:  # Only show highly active words
                f.write(f"Word {i:2d}: {activity:.4f} ({activity*100:.2f}%)\n")
    
    print("   Saved summary: id_od_analysis_summary.txt")
    
    print("\n" + "="*60)
    print("ANALYSIS COMPLETE!")
    print("="*60)
    print("\nKey Findings:")
    print(f"- Best input difference: {best_characteristics[0]['input_word']},{best_characteristics[0]['input_bit']}")
    print(f"- Effectiveness score: {best_characteristics[0]['effectiveness']:.3f}")
    print(f"- Most active output word: {np.argmax(word_activity)} ({word_activity.max():.3f})")
    print(f"- Total characteristics analyzed: {len(best_characteristics)}")

if __name__ == "__main__":
    main()