import multiprocessing, subprocess, time

def RunSingleProcess():
    result = subprocess.run('Binaries\Onirim.exe AIPlayer', shell=True)
    return result.returncode

def RunAllProcesses(run_count):
    results = []
    with multiprocessing.Pool(processes=8) as pool:
        for i in range(run_count):
            pool.apply_async(RunSingleProcess, callback=lambda x: results.append(x))
        while len(results) < run_count:
            time.sleep(0.1)
    return results

if __name__ == '__main__':
    runs = int(input('run count: '))
    t = time.monotonic()
    results = RunAllProcesses(runs)
    
    wins = results.count(1)
    print('')
    print(f'played: {len(results)}')
    print(f'wins: {wins}')
    print(f'losses: {len(results) - wins}')
    print(f'win rate: {(wins/len(results))*100:.2f}%')
    input(f'\nfinished in {time.monotonic() - t:.2f} seconds')
