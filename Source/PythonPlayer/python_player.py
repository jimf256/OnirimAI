import random
from parse_game_state import ParseGameState, GetColorByte

debug_printing = False

def DebugPrint(s):
    if debug_printing:
        print(s)

def HandleGameStarted(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: game started:\n{state}')
    return None

def HandleGameEnded(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: game ended:\n{state}')
    return None


# resolve a turn action
# must return a bytearray of length 2
# first byte is a value 0/1: play card/discard card
# second byte is a value 0-4: hand card index
def HandleResolveTurn(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: resolve turn:\n{state}')

    # find the current state of the labrynth, how many cards of the same colour are currently on the end    
    lab_card = state.labrynth[-1] if state.labrynth else None
    lab_color = state.labrynth[-1][0] if state.labrynth else None
    lab_count = 0
    if lab_color != None:
        for card in state.labrynth[::-1]:
            if card[0] == lab_color:
                lab_count += 1
            else:
                break

    # can we play a card to get a door right now (third in a series)?
    if lab_count % 3 == 2:
        for i,card in enumerate(state.hand):
            if card[0] == lab_color and card[1] != lab_card[1]:
                return bytearray([0, i])

    # can we play a sun/moon to get 2/3 of a color
    if lab_count % 3 == 1:
        for i,card in enumerate(state.hand):
            if card[0] == lab_color and card[1] != lab_card[1] and card[1] != 'K':
                return bytearray([0, i])

    # do we have a sun/moon of one color in hand and can play one of them?
    suns = {'R':0, 'B':0, 'G':0, 'Y':0}
    moons = {'R':0, 'B':0, 'G':0, 'Y':0}
    for card in state.hand:
        if card[1] == 'S':
            suns[card[0]] += 1
        elif card[1] == 'M':
            moons[card[0]] += 1
    for c in ['R', 'B', 'G', 'Y']:
        if suns[c] > 0 and moons[c] > 0:
            t = 'S' if lab_card == None or lab_card[1] != 'S' else 'M'
            for i,card in enumerate(state.hand):
                if card[0] == c and card[1] == t:
                    return bytearray([0, i])

    # can we discard a sun/moon
    for i,card in enumerate(state.hand):
        if card[1] == 'S' or card[1] == 'M':
            return bytearray([1, i])

    # can we discard a key
    for i,card in enumerate(state.hand):
        if card[1] == 'K':
            return bytearray([1, i])         

    # fallback: discard first card
    return bytearray([1, 0])


# resolve a nightmare action
# must return a bytearray of length 2
# first byte is a value 0-3: destroy a door/discard a key/discard hand/discard from deck
# second byte is a value 0-4: hand card index
def HandleResolveNightmare(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: resolve nightmare:\n{state}')

    # if we have a key in hand, discard it, otherwise discard the hand
    key_color = None
    for card in state.hand:
        if card[1] == 'K':
            choice = 1
            key_color = GetColorByte(card[0])
            break

    if key_color is None:
        choice = 2
        key_color = 0

    return bytearray([choice, key_color])


# resolve a door card action
# must return a bytearray of length 1
# first byte is a value 0/1: use key/keep key
def HandleResolveDoorCard(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: resolve door card:\n{state}')

    # always use a key
    choice = 0;
    return bytearray([choice])


# resolve a premonition action
# must return a bytearray of length len(state.premonition)
# state.premonition contains 0 to 5 cards which are the cards on the top of the deck
# the returned values are card indexes, from 0 to 4, representing the new ordering of those cards
# for example, to reverse the order of the cards, return [4,3,2,1,0]
# the last card in the list is the one that will be discarded
def HandleResolvePremonition(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: resolve premonition:\n{state}')

    # prioritize keeping keys, then regular cards, then doors, then nightmares
    # prioritize discarding nightmares, regular cards, keys then doors
    # only discard a door if there is no other choice
    keys = []
    doors = []
    nightmares = []
    others = []
    for i,card in enumerate(state.premonition):
        if card[1] is 'K':
            keys.append((card, i))
        elif card[1] is 'D':
            doors.append((card, i))
        elif card == 'NM':
            nightmares.append((card, i))
        else:
            others.append((card, i))

    order = []
    if len(doors) == len(state.premonition) or len(keys) == len(state.premonition):
        order = list(range(len(state.premonition)))
    elif len(doors) + len(keys) == len(state.premonition):
        order = [x[1] for x in keys[1:]] + [x[1] for x in doors] + [keys[0][1]]
    elif len(nightmares) > 0:
        order = [x[1] for x in keys] + [x[1] for x in others] + [x[1] for x in doors] + [x[1] for x in nightmares]
    else:
        order = [x[1] for x in keys] + [x[1] for x in others[1:]] + [x[1] for x in doors] + [others[0][1]]

    DebugPrint(order)
    DebugPrint([state.premonition[x] for x in order])
    return bytearray(order)
