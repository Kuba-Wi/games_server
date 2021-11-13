## Dependencies
To build the snake_app on Android, you need to download one dependecy by yourself.
The required dependecy is [Boost for Android](https://github.com/moritz-wundke/Boost-for-Android).
Then run CMake with flags:
```
cmake -DANDROID_BOOST_PATH=<Path to Boost for Android> ..
```
For example:
```
cmake -DANDROID_BOOST_PATH=$HOME/Boost-for-Android/boost_1_76_0 ..
```
