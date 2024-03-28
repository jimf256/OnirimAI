### OnirimAI is an AI testing/development framework for the card game [Onirim](https://boardgamegeek.com/boardgame/71836/onirim) with the following features:
- plugin architecture for different AI player implementations
- AI players can be written in either C++ or Python (includes a working example of each)
- Interactive console-based player for testing game logic
- scripts for running configurable AI players and collating the results (wins/losses/errors)

On my very modest CPU the included basic C++ AI can achieve 10000 runs of the game per second

### Requirements:
- Visual Studio 2022
- Python 3.7+ for the PythonPlayer implementation and some of the helper scripts
