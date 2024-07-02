
# **AppPealing**

An Xposed module that disables [Inka AppSealing](https://www.appsealing.com/), a popular anti-cheat and anti-root solution.

**Features**

- Disable root detection: Prevents Inka AppSealing from detecting your rooted device.
- Disable cheat detection: Disables all cheat detection mechanisms implemented by Inka AppSealing.
- Dump encrypted Dex files: Allows you to dump and decrypt the encrypted Dex files used by Inka AppSealing, which can be useful for debugging and reverse engineering.

**Compatibility**

AppPealing has been tested on Android 14 with [LSPosed](https://github.com/LSPosed/LSPosed) against Inka AppSealing build version: 2.32.0.0.

**Installation**
1. Install Magisk following [this guide](https://topjohnwu.github.io/Magisk/install.html), and enable Zygisk.
1. Install [LSPosed](https://github.com/LSPosed/LSPosed).
1. Download the AppPealing from [releases](https://github.com/LEAGUE-OF-NINE/AppPealing/releases).
1. Enable AppPealing in LSPosed for apps you want to neuter AppSealing for. 
1. Reboot your device.

**Usage**

Once installed, AppPealing will automatically disable root detection and cheat detection by Inka AppSealing. 
Dumped Dex files are stored in `/data/user/0/(package name)/apppeal_dump`. These can be copied to the device's internal storage by:
```
adb shell
su
cp -r /data/user/0/(package name)/apppeal_dump /storage/emulated/0/
``` 

**Disclaimer**

This module is intended for debugging and testing purposes only. Use at your own risk!

**License**

AppPealing is licensed under ISC. 

**Credit**

@ARandomPerson7 for documenting how Inka AppSealing can be bypassed in his [repository](https://github.com/ARandomPerson7/Appsealing-Reversal) and also helping me out over Discord.
