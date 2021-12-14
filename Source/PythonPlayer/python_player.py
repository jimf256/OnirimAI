import random
from parse_game_state import ParseGameState, GetColorByte

debug_printing = False

def DebugPrint(s):
    if debug_printing:
        print(s)

def HandleGameStarted(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: game started:\n{state}')
    return None

def HandleGameEnded(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: game ended:\n{state}')
    return None


# resolve a turn action
# must return a bytearray of length 2
# first byte is a value 0/1: play card/discard card
# second byte is a value 0-4: hand card index
def HandleResolveTurn(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: resolve turn:\n{state}')

    colors_prioritized = sorted(['R', 'B', 'G', 'Y'], key=lambda x: state.doors[x])
    colors_completed = [x for x in ['R','B','G','Y'] if state.doors[x] == 2]

    suns = {'R':0, 'B':0, 'G':0, 'Y':0}
    moons = {'R':0, 'B':0, 'G':0, 'Y':0}
    keys = {'R':0, 'B':0, 'G':0, 'Y':0}
    for card in state.hand:
        if card[1] == 'S':
            suns[card[0]] += 1
        elif card[1] == 'M':
            moons[card[0]] += 1
        elif card[1] == 'K':
            keys[card[0]] += 1
                              
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

    result = None
    
    # can we play a card to get a door right now (third in a series)?
    if lab_count % 3 == 2 and state.doors[lab_card[0]] < 2:
        for i,card in enumerate(state.hand):
            if card[0] == lab_color and card[1] != lab_card[1]:
                result = bytearray([0, i])

    # can we play a sun/moon to get 2/3 of a color
    if result is None:
        if lab_count % 3 == 1 and state.doors[lab_card[0]] < 2:
            for i,card in enumerate(state.hand):
                if card[0] == lab_color and card[1] != lab_card[1] and card[1] != 'K':
                    result = bytearray([0, i])

    # can we play a key to get 2/3 of a color, already having a third card ready to play afterwards
    if result is None:
        if lab_count % 3 == 1 and state.doors[lab_card[0]] < 2:
            for i,card in enumerate(state.hand):
                if card[0] == lab_color and card[1] != lab_card[1] and card[1] == 'K' and (suns[card[0]] + moons[card[0]]) > 0:
                    result = bytearray([0, i])

    # do we have a sun/moon of one color in hand and can play one of them?
    if result is None:
        if lab_count % 3 < 2:
            for c in colors_prioritized:
                if state.doors[c] > 1:
                    continue
                if suns[c] > 0 and moons[c] > 0:
                    t = 'S' if lab_card == None or lab_card[1] != 'S' else 'M'
                    for i,card in enumerate(state.hand):
                        if card[0] == c and card[1] == t:
                            result = bytearray([0, i])

    # discard sun/moon cards from already completed colors
    if result is None:
        for c in colors_completed:
            for i,card in enumerate(state.hand):
                if card[0] == c and (card[1] == 'S' or card[1] == 'M'):
                    result = bytearray([1, i])

    # if we have two suns or two moons of a color, just play one if possible
    if result is None:
        if lab_count % 3 == 0:
            for c in colors_prioritized:
                if suns[c] > 1 and (lab_card == None or lab_card[1] != 'S'):
                    for i,card in enumerate(state.hand):
                        if card[0] == c and card[1] == 'S':
                            result = bytearray([0, i])
                    break;
                if moons[c] > 1 and (lab_card == None or lab_card[1] != 'M'):
                    for i,card in enumerate(state.hand):
                        if card[0] == c and card[1] == 'M':
                            result = bytearray([0, i])
                    break;

    # discard a key
    if result is None:
        if lab_count % 3 == 2:
            for c in reversed(colors_prioritized):
                for i,card in enumerate(state.hand):
                    if card[0] == c and card[1] == 'K':
                        result = bytearray([1, i])

    # discard a sun
    if result is None:
        for c in reversed(colors_prioritized):
            for i,card in enumerate(state.hand):
                if card[0] == c and card[1] == 'S':
                    result = bytearray([1, i])

    # discard a moon
    if result is None:
        for c in reversed(colors_prioritized):
            for i,card in enumerate(state.hand):
                if card[0] == c and card[1] == 'M':
                    result = bytearray([1, i])

    # fallback: discard first card
    if result is None:
        result = bytearray([1, 0])

    DebugPrint(f'chose: {list(result)}')
    return result


# resolve a nightmare action
# must return a bytearray of length 2
# first byte is a value 0-3: destroy a door/discard a key/discard hand/discard from deck
# second byte is a value 0-4: hand card index
def HandleResolveNightmare(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: resolve nightmare:\n{state}')

    lab_card = state.labrynth[-1] if state.labrynth else None
    lab_color = state.labrynth[-1][0] if state.labrynth else None
    lab_count = 0
    if lab_color != None:
        for card in state.labrynth[::-1]:
            if card[0] == lab_color:
                lab_count += 1
            else:
                break

    # if we have a key in hand, discard it
    key_color = None
    colors_prioritized = sorted(['R', 'B', 'G', 'Y'], key=lambda x: state.doors[x])
    for c in reversed(colors_prioritized):
        for card in state.hand:
            if card[1] == 'K' and card[0] == c:
                choice = 1
                key_color = GetColorByte(card[0])
                return bytearray([choice, key_color])

    suns = {'R':0, 'B':0, 'G':0, 'Y':0}
    moons = {'R':0, 'B':0, 'G':0, 'Y':0}
    keys = {'R':0, 'B':0, 'G':0, 'Y':0}
    for card in state.hand:
        if card[1] == 'S':
            suns[card[0]] += 1
        elif card[1] == 'M':
            moons[card[0]] += 1
        elif card[1] == 'K':
            keys[card[0]] += 1

    # if we have the cards in hand to make a door, discard deck
    if lab_count % 3 == 2:
        for card in state.hand:
            if card[0] == lab_card[0] and card[1] != lab_card[1]:
                return bytearray([3, 0])

    if lab_count % 3 == 1 and suns[lab_card[0]] > 0 and moons[lab_card[0]] > 0:
        return bytearray([3, 0])

    # otherwise just discard the hand
    return bytearray([2, 0])


# resolve a door card action
# must return a bytearray of length 1
# first byte is a value 0/1: use key/keep key
def HandleResolveDoorCard(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: resolve door card:\n{state}')

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
    DebugPrint(f'\npython AI: resolve premonition:\n{state}')

    colors_prioritized = sorted(['R', 'B', 'G', 'Y'], key=lambda x: state.doors[x])
    lab_card = state.labrynth[-1] if state.labrynth else None
    lab_color = state.labrynth[-1][0] if state.labrynth else None
    lab_count = 0
    if lab_color != None:
        for card in state.labrynth[::-1]:
            if card[0] == lab_color:
                lab_count += 1
            else:
                break

    doors_in_premonition = [x[0] for x in state.premonition if x[1] == 'D']
    available_keys = set(x[0] for x in state.premonition + state.hand if x[1] == 'K')

    prioritized = [[x[0], x[1], i, 1.0] for i,x in enumerate(state.premonition)]
    type_priorities = {'K':10, 'M':5, 'S': 3}
    for card in prioritized:
        if ''.join(card[0:2]) == 'NM':
            card[3] = -1000.0
        elif card[1] == 'K' and card[0] in doors_in_premonition:
            card[3] = 1000.0
        elif card[1] == 'D' and card[0] in available_keys:
            card[3] = 999.0
        elif lab_count % 3 == 2 and card[1] != 'D' and card[1] != 'K' and card[0] == lab_card[0] and card[1] != lab_card[1]:
            card[3] = 550.0
        elif lab_count % 3 == 2 and card[1] != 'D' and card[0] == lab_card[0] and card[1] != lab_card[1]:
            card[3] = 500.0
        elif lab_count % 3 == 1 and card[1] != 'D' and card[0] == lab_card[0] and card[1] != lab_card[1]:
            card[3] = 100.0
        elif card[1] == 'K':
            card[3] = 50.0
        elif card[1] == 'D':
            card[3] = 40.0
        elif card[1] in type_priorities:
            card[3] = type_priorities[card[1]]
        else:
            card[3] = 1.0

    prioritized.sort(reverse=True, key=lambda x: x[3])
    order = [x[2] for x in prioritized]
    reordered = [state.premonition[x] for x in order]
    DebugPrint(f'reordered: {reordered}')
    return bytearray(order)
