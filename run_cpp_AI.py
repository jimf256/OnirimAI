import subprocess, time, os

results_file = 'results.log'

if __name__ == '__main__':
    if os.path.exists(results_file) and os.path.isfile(results_file):
        os.remove(results_file)

    runs = int(input('run count: '))
    t = time.monotonic()
    devnull = subprocess.DEVNULL
    command = f'Binaries\Onirim.exe AIPlayer runs={runs}'
    proc = subprocess.run(command, stdin=devnull, stdout=devnull, stderr=devnull, shell=False)

    results = {}
    with open(results_file, 'r') as f:
        for line in [x.strip() for x in f.readlines()]:
            if line and len(line) > 2:
                results[int(line[0])] = int(line[2:].strip())

    wins = results.get(1, 0)
    plays = sum(results.values())
    print('\n::: results :::')
    print(f'played:   {plays}')
    print(f'wins:     {wins}')
    print(f'losses:   {plays - wins}')
    print(f'win rate: {(wins/plays)*100:.2f}%')
    input(f'\nfinished in {time.monotonic() - t:.2f} seconds')
