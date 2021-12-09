import subprocess, os, time, python_ai_logic

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

def RunGameInstance():
    RemoveFile(cpp_file)
    RemoveFile(python_file)
    RemoveFile(cpp_waiting_signal)
    RemoveFile(python_waiting_signal)
    RemoveFile(event_started_signal)
    RemoveFile(event_completed_signal)
    RemoveFile(game_over_signal)
    
    proc = subprocess.Popen('Binaries\Onirim_d.exe PythonPlayer', shell=True)
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
                        elif data[0].strip() == 'ResolveTurn':
                            ResolveTurn(data)
                        elif data[0].strip() == 'ResolveNightmare':
                            ResolveNightmare(data)
                        elif data[0].strip() == 'ResolveDoorCard':
                            ResolveDoorCard(data)
                        elif data[0].strip() == 'ResolvePremonition':
                            ResolvePremonition(data)
                        elif data[0].strip() == 'OnGameOver':
                            OnGameEnded(data)
                            finished = True
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

    while True:
        if os.path.exists(game_over_signal) and os.path.isfile(game_over_signal):
            break;
    
    RemoveFile(cpp_file)
    RemoveFile(python_file)
    RemoveFile(cpp_waiting_signal)
    RemoveFile(python_waiting_signal)
    RemoveFile(event_started_signal)
    RemoveFile(event_completed_signal)
    RemoveFile(game_over_signal)


if __name__ == '__main__':
    t = time.monotonic()
    for i in range(int(input('number of runs: '))):
        RunGameInstance()
    input(f'completed in {time.monotonic() - t:.2f} seconds')
