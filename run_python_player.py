import subprocess, os, sys, time
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'Source', 'PythonAI'))
import python_ai_logic

cpp_file = 'cpp_data.txt'
python_file = 'python_data.txt'
cpp_waiting_signal = 'cpp_waiting_signal.txt'
python_waiting_signal = 'python_waiting_signal.txt'
event_started_signal = 'event_started_signal.txt'
event_completed_signal = 'event_completed_signal.txt'
game_over_signal = 'game_over_signal.txt'

def RemoveFile(file):
    if os.path.exists(file) and os.path.isfile(file):
        os.remove(file)

def CreateFile(file):
    with open(file, 'w') as f:
        pass

def WritePythonData(data):
    with open(python_file, 'w') as f:
        f.write(data if data else '')

def OnGameStarted(data):
    python_ai_logic.HandleGameStarted(data)

def ResolveTurn(data):
    WritePythonData(python_ai_logic.HandleResolveTurn(data))

def ResolveNightmare(data):
    WritePythonData(python_ai_logic.HandleResolveNightmare(data))

def ResolveDoorCard(data):
    WritePythonData(python_ai_logic.HandleResolveDoorCard(data))

def ResolvePremonition(data):
    WritePythonData(python_ai_logic.HandleResolvePremonition(data))

def OnGameEnded(data):
    python_ai_logic.HandleGameEnded(data)

def RunGameInstances(runs):
    RemoveFile(cpp_file)
    RemoveFile(python_file)
    RemoveFile(cpp_waiting_signal)
    RemoveFile(python_waiting_signal)
    RemoveFile(event_started_signal)
    RemoveFile(event_completed_signal)
    RemoveFile(game_over_signal)

    devnull = subprocess.DEVNULL
    command = f'Binaries\Onirim.exe PythonPlayer runs={runs}'
    proc = subprocess.Popen(command, stdin=devnull, stdout=devnull, stderr=devnull, shell=False)
    runs_completed = 0
    try:
        while proc.poll() is None and runs_completed < runs:
            finished = False
            while not finished:
                while True:
                    if os.path.exists(event_started_signal) and os.path.isfile(event_started_signal):
                        RemoveFile(event_completed_signal)
                        CreateFile(python_waiting_signal)
                        break
                while True:
                    if os.path.exists(cpp_waiting_signal) and os.path.isfile(cpp_waiting_signal):
                        if os.path.exists(cpp_file) and os.path.isfile(cpp_file):
                            with open(cpp_file, 'r') as f:
                                data = f.readlines();
                                if not data:
                                    finished = True
                                    print('empty data received')
                                    proc.kill()
                                    break
                                elif data[0].strip() == 'OnGameStarted':
                                    OnGameStarted(data[1:])
                                elif data[0].strip() == 'ResolveTurn':
                                    ResolveTurn(data[1:])
                                elif data[0].strip() == 'ResolveNightmare':
                                    ResolveNightmare(data[1:])
                                elif data[0].strip() == 'ResolveDoorCard':
                                    ResolveDoorCard(data[1:])
                                elif data[0].strip() == 'ResolvePremonition':
                                    ResolvePremonition(data[1:])
                                elif data[0].strip() == 'OnGameEnded':
                                    OnGameEnded(data[1:])
                                    finished = True
                                    runs_completed += 1
                                else:
                                    finished = True
                                    print(f'invalid data received:\n{data}')
                                    proc.kill()
                                    break
                            RemoveFile(python_waiting_signal)
                            CreateFile(event_completed_signal)
                            break
                        else:
                            finished = True
                            print('failed to open cpp data file')
                            proc.kill()
                            break
    except Exception as e:
        print(f'exception: {e}')
        proc.kill()        

    proc.wait()    
    RemoveFile(cpp_file)
    RemoveFile(python_file)
    RemoveFile(cpp_waiting_signal)
    RemoveFile(python_waiting_signal)
    RemoveFile(event_started_signal)
    RemoveFile(event_completed_signal)
    RemoveFile(game_over_signal)


if __name__ == '__main__':
    runs = int(input('run count: '))
    t = time.monotonic()
    RunGameInstances(runs)
    input(f'completed in {time.monotonic() - t:.2f} seconds')
