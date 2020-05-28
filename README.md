# enqueueee
Program that enqueues songs in Winamp (in 2004) without destroying the existing playlist

Winamp's forums are still operational! http://forums.winamp.com/showthread.php?postid=1522325

```
Usage: Enqueueee [-abe | --help ] file
-a      Enqueue song immediately after currently playing song
-b      Enqueue song before currently playing song
-e      Enqueue song at the end of playlist
```

NOTE: WINAMP MUST BE ON

To get this to "work" as default, I went into Folder Options->File Types and changed the default program that handles mp3s to be Enqueueee.  I then set the application used to perform action to "C:\Program Files\Winamp\Enqueueee.exe" -b "%1".
This would be a sweet feature for Winamp 5.0.next 
Thanks to torsius' and WinampMagic for helping out a n00b!!!


Sample usage

`-e` Enqueue song at the end of playlist (does the same thing that Joonas's does)
  ```
  c:\> Enqueueee -e "full\path\song.mp3"
  ```

`-b` Enqueue song before currently playing song 

  ```
  c:\> Enqueueee -b "full\path\song.mp3"
  
  BEFORE       AFTER
  trk1         trk1
  trk2         trk2
  > trk3       > song.mp3
  trk4         trk3
               trk4
  ```

`-a` Enqueue song immediately after currently playing song

  ```
  c:\>Enqueueee -a "full\path\song.mp3"
  BEFORE       AFTER
  trk1         trk1
  trk2         trk2
  > trk3       trk3
  trk4         > song.mp3
               trk4
  ```
  
Playlists are fun too: 

  ```
  # Assume that songs.m3u is {song1.mp3, song2.mp3}
  c:\> Enqueueee -e "full\path\songs.m3u"
  ```
