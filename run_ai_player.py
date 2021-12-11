import subprocess, time

if __name__ == '__main__':
    runs = int(input('run count: '))
    t = time.monotonic()
    proc = subprocess.run(f'Binaries\Onirim.exe AIPlayer runs={runs}', shell=True)

    results = {}
    with open('results.log', 'r') as f:
        for line in [x.strip() for x in f.readlines()]:
            if line and len(line) > 2:
                results[int(line[0])] = int(line[2:].strip())

    wins = results[1]
    plays = sum(results.values())
    print('\n::: results :::')
    print(f'played:   {plays}')
    print(f'wins:     {wins}')
    print(f'losses:   {plays - wins}')
    print(f'win rate: {(wins/plays)*100:.2f}%')
    input(f'\nfinished in {time.monotonic() - t:.2f} seconds')
