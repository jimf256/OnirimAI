import subprocess, time, os

results_file = 'results.log'

if __name__ == '__main__':

    # determine which AI module to import via the cpp_player_config.ini file
    ai_module_name = 'AIPlayer'
    config_file = 'cpp_player_config.ini'
    with open(config_file, 'r') as f:
        ai_module_name = f.readline().strip()

    if os.path.exists(results_file) and os.path.isfile(results_file):
        os.remove(results_file)

    runs = int(input('run count: '))
    t = time.monotonic()
    devnull = subprocess.DEVNULL
    command = f'Binaries\\Onirim.exe {ai_module_name} runs={runs}'
    proc = subprocess.run(command, stdin=devnull, stdout=devnull, stderr=devnull, shell=False)

    results = {}
    with open(results_file, 'r') as f:
        for line in [x.strip() for x in f.readlines()]:
            if line and len(line) > 2:
                results[int(line[0])] = int(line[2:].strip())

    wins = results.get(1, 0)
    losses = results.get(2, 0) + results.get(3, 0)
    errors = results.get(4, 0)
    plays = sum(results.values())
    print('\n::: results :::')
    print(f'played:   {plays}')
    print(f'wins:     {wins}')
    print(f'losses:   {losses}')
    print(f'errors:   {errors}')
    print(f'win rate: {(wins/plays)*100:.2f}%')
    input(f'\nfinished in {time.monotonic() - t:.2f} seconds')
