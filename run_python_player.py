import subprocess, os, sys, time, mmap, traceback
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'Source', 'PythonAI'))
import python_ai_logic

data_size = 1024
data_file = 'shared_data.txt'
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

def ExchangeData(mmap_buf, ai_func):
    lines = [line.strip('\0') for line in str(mmap_buf[:], 'utf-8').split('\n') if line.strip('\0')]
    python_data = ai_func(lines[1:])
    if python_data:
        mmap_buf.seek(0)
        mmap_buf.write(python_data.decode('utf-8'))
        mmap_buf.flush()

def RunGameInstances(runs):
    RemoveFile(data_file)
    RemoveFile(cpp_waiting_signal)
    RemoveFile(python_waiting_signal)
    RemoveFile(event_started_signal)
    RemoveFile(event_completed_signal)
    RemoveFile(game_over_signal)

    if not os.path.exists(data_file):
        with open(data_file, 'wb') as f:
            f.write(b'\0' * data_size)

    with open(data_file, 'r+b') as f:
        mmap_buf = mmap.mmap(f.fileno(), data_size, access=mmap.ACCESS_WRITE)
        mmap_buf.flush()
        mmap_buf.seek(0)

        devnull = subprocess.DEVNULL
        command = f'Binaries\Onirim.exe PythonPlayer runs={runs}'
        proc = subprocess.Popen(command, stdin=devnull, stdout=devnull, stderr=devnull, shell=False)
        runs_completed = 0
        error_encountered = False
        try:
            while not error_encountered and runs_completed < runs:
                finished = False
                while not finished and not error_encountered:
                    while True:
                        if os.path.exists(event_started_signal) and os.path.isfile(event_started_signal):
                            RemoveFile(event_completed_signal)
                            CreateFile(python_waiting_signal)
                            break
                    while True:
                        if os.path.exists(cpp_waiting_signal) and os.path.isfile(cpp_waiting_signal):
                            mmap_buf.seek(0)
                            event = str(mmap_buf.readline(), 'utf-8').strip()
                            if not event:
                                error_encountered = True
                                print('empty data received')
                                proc.terminate()
                                break
                            elif event == 'OnGameStarted':
                                ExchangeData(mmap_buf, python_ai_logic.HandleGameStarted)
                            elif event == 'ResolveTurn':
                                ExchangeData(mmap_buf, python_ai_logic.HandleResolveTurn)
                            elif event == 'ResolveNightmare':
                                ExchangeData(mmap_buf, python_ai_logic.HandleResolveNightmare)
                            elif event == 'ResolveDoorCard':
                                ExchangeData(mmap_buf, python_ai_logic.HandleResolveDoorCard)
                            elif event == 'ResolvePremonition':
                                ExchangeData(mmap_buf, python_ai_logic.HandleResolvePremonition)
                            elif event == 'OnGameEnded':
                                ExchangeData(mmap_buf, python_ai_logic.HandleGameEnded)
                                finished = True
                                runs_completed += 1
                            else:
                                error_encountered = True
                                print(f'invalid data received:\n{event}')
                                proc.terminate()
                                break
                            RemoveFile(python_waiting_signal)
                            CreateFile(event_completed_signal)
                            break
                while proc.poll() !=  None:
                    print('waiting for process to die')

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
    RemoveFile(data_file)
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
