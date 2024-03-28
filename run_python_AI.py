import subprocess, os, sys, time, mmap, traceback, ctypes, importlib

# determine which AI script to import via the python_player_config.ini file
ai_module_name = 'normal_ai'
config_file = 'python_player_config.ini'
with open(config_file, 'r') as f:
    ai_module_name = f.readline().strip()
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'Source', 'PythonPlayer'))
ai_module = importlib.import_module(ai_module_name)

debug = False
data_size = 1024
data_file = 'shared_data.txt'
results_file = 'results.log'

def ExchangeData(mmap_buf, ai_func):
    lines = [line.strip('\0') for line in str(mmap_buf[:], 'utf-8').split('\n') if line.strip('\0')]
    python_data = ai_func(lines[1:])
    if python_data:
        mmap_buf.seek(0)
        mmap_buf.write_byte(len(python_data))
        mmap_buf.write(python_data)
        mmap_buf.flush()

def RunGameInstances(runs):
    if os.path.exists(data_file) and os.path.isfile(data_file):
        os.remove(data_file)
    if os.path.exists(results_file) and os.path.isfile(results_file):
        os.remove(results_file)
    with open(data_file, 'wb') as f:
        f.write(b'\0' * data_size)

    with open(data_file, 'r+b') as f:
        mmap_buf = mmap.mmap(f.fileno(), data_size, access=mmap.ACCESS_WRITE)
        mmap_buf.flush()
        mmap_buf.seek(0)

        ai_module.OnSimulationStarted()
        
        devnull = subprocess.DEVNULL
        debug_suffix = '_d' if debug else ''
        command = f'Binaries\\Onirim{debug_suffix}.exe PythonPlayer runs={runs}'
        proc = subprocess.Popen(command, stdin=devnull, stdout=devnull, stderr=devnull, shell=False)
        if debug:
            input('attach debugger then press return')

        runs_completed = 0
        error_encountered = False
        try:
            lib = ctypes.CDLL(f'Libraries\\PythonPlayer{debug_suffix}.dll')

            while not error_encountered and runs_completed < runs:
                finished = False
                
                while not finished and not error_encountered:
                    lib.WaitForCppData()
                    mmap_buf.seek(0)
                    event = str(mmap_buf.readline(), 'utf-8').strip()
                    if not event:
                        error_encountered = True
                        print('empty data received')
                        proc.terminate()
                        lib.SignalPythonData()
                        break
                    elif event == 'OnGameStarted':
                        ExchangeData(mmap_buf, ai_module.HandleGameStarted)
                    elif event == 'ResolveTurn':
                        ExchangeData(mmap_buf, ai_module.HandleResolveTurn)
                    elif event == 'ResolveNightmare':
                        ExchangeData(mmap_buf, ai_module.HandleResolveNightmare)
                    elif event == 'ResolveDoorCard':
                        ExchangeData(mmap_buf, ai_module.HandleResolveDoorCard)
                    elif event == 'ResolvePremonition':
                        ExchangeData(mmap_buf, ai_module.HandleResolvePremonition)
                    elif event == 'OnGameEnded':
                        ExchangeData(mmap_buf, ai_module.HandleGameEnded)
                        finished = True
                        runs_completed += 1
                    else:
                        error_encountered = True
                        print(f'invalid data received:\n{event}')
                        proc.terminate()
                        lib.SignalPythonData()
                        break
                    lib.SignalPythonData()
                    
            while proc.poll() ==  None:
                pass

        except Exception as e:
            print(f'exception: {e}\n')
            traceback.print_tb(sys.exc_info()[2])
            proc.terminate()
        finally:
            mmap_buf.flush()
            mmap_buf.close()

    if error_encountered:
        print('there was an error!')

    proc.wait()    
    if os.path.exists(data_file) and os.path.isfile(data_file):
        os.remove(data_file)

    ai_module.OnSimulationEnded()
    
    results = {}
    with open('results.log', 'r') as f:
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


if __name__ == '__main__':
    runs = int(input('run count: '))
    t = time.monotonic()
    RunGameInstances(runs)
    input(f'finished in {time.monotonic() - t:.2f} seconds')
