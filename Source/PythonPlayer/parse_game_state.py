
class GameState:
    def __str__(self):
        s = 'GameState:\n'
        for x in self.__dict__:
            s += f'  {x}: {self.__dict__[x]}\n'
        return s


def ParseGameState(data):
    state = GameState()
    for entry in data:
        equals = entry.find('=')
        if equals == -1:
            print('invalid gamestate entry found:', entry)
            continue
        key = entry[:equals]
        value = entry[equals+1:]
        if key.lower() == 'result':
            state.result = int(value)
        elif key.lower() == 'deck_remaining':
            state.deck_remaining = int(value)
        elif key.lower() == 'nm_remaining':
            state.nightmares_remaining = int(value)
        elif key.lower() == 'labrynth':
            state.labrynth = [x.upper() for x in value.split(',') if x]
        elif key.lower() == 'limbo':
            state.limbo = [x.upper() for x in value.split(',') if x]
        elif key.lower() == 'hand':
            state.hand = [x.upper() for x in value.split(',') if x]
        elif key.lower() == 'doors':
            state.doors = {x[0].upper():int(x[1]) for x in value.split(',') if len(x) == 2}
        elif key.lower() == 'discard':
            state.discard = {x[0:2].upper():int(x[3]) for x in value.split(',') if len(x) == 4}
        elif key.lower() == 'premonition':
            state.premonition = [x.upper() for x in value.split(',') if x]

    return state
