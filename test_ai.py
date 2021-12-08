import subprocess

run_count = int(input('run count: '))

results = {'win': 0, 'loss': 0}
for i in range(run_count):
    result = subprocess.run('Binaries\Onirim_d.exe AIPlayer')
    if result.returncode == 1:
        results['win'] += 1
    else:
        results['loss'] += 1

print(f'total runs: {run_count}')
print(f'wins: {results["win"]}')
print(f'losses: {results["loss"]}')
print(f'win rate: {(results["win"]/run_count)*100:.2f}%')
input('finished')
