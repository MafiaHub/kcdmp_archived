# KCD Multiplayer Proof of Concept

This is a proof of concept for a multiplayer mod for Kingdom Come: Deliverance. It is unfinished, but due to popular demand, we are releasing the source code.

Expect bugs, crashes, and other issues. This is not a finished product, and we do not provide support for it. We are releasing it as-is, and we are not responsible for any damage it may cause to your game or your computer.

You might observe bad programming practices, hacky code and lot's of shortcuts to achieve what we have presented back in 2018. This code is **NOT** suitable for further development and is only shared with you for educational purposes.

After all, this is a **proof of concept** code from back in **2018**. We have learned a lot since then and we would not write code like this today. Have fun!

## Features

- Synchronization of player movement and animations
- Horse riding synchronization

Yup, that's it. We never got to the point of implementing anything else.

## How to use

1. Open KDCPlayground.sln in Visual Studio 2022 or later.
2. Build the solution.
3. Copy the contents of the `x64/[Debug or Release]` folder to your Kingdom Come: Deliverance installation directory.
4. Run the **SimpleServer.exe** executable.
5. Run the game and inject **KDCPlayground.dll** using your favorite DLL injector.

The client automatically joins the server on localhost. If you want to connect to a different server, you can modify the `multiplayer.json` file upon the first launch to specify the server's IP address.

## Known issues

We don't remember. On top of that, it is very likely this build does **NOT** work with the latest version of the game. Again, we provide no support whatsoever for this project.

## Path forward

We are the developers behind [MafiaHub Framework](https://github.com/MafiaHub/Framework) and we currently work on a multiplayer mod for [Mafia Definitive Edition](https://github.com/MafiaHub/MafiaMP). While we don't have the manpower to develop a multiplayer mod for Kingdom Come: Deliverance, we are open to collaboration with anyone who is interested in continuing this project. We would provide guidance and initial codebase implementation that would follow modern standards and practices we have learned over the years.

If you are a C/C++ developer with deep understanding of game memory layouts and reverse engineering, as well as the game's internals, and you are interested in working on a multiplayer mod for Kingdom Come: Deliverance, feel free to contact us.

This repository is archived and we will not be accepting any pull requests or issues.

## License

The code is licensed under the [MafiaHub OSS](LICENSE.txt) license.

The 5th clause forbids any forks of this project. We are not interested in seeing any further development on this codebase. Please, do not create new MP projects based on this code. We are open to collaboration on a new project, but we will not support any forks of this codebase.
