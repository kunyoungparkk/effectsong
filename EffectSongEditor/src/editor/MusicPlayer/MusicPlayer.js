import { useRef, useState, useEffect } from "react";
import AudioPlayer from "react-h5-audio-player";
import 'react-h5-audio-player/lib/styles.css';

const MusicPlayer = ({ module }) => {
  const audioRef = useRef(null);
  useEffect(() => {
  }, []);

  useEffect(() => {
    const interval = setInterval(() => {
      if (module) {
        module.loop(audioRef.current.audio.current.currentTime, audioRef.current.isPlaying())
      }
    }, 1000.0 / 60.0);
  }, [module]);

  const action=(eStr)=>{
    console.log(eStr);
  }
  return (
    <AudioPlayer
      ref={audioRef}
      volume={0.8}
      showSkipControls
      progressUpdateInterval={100}
      src="test.wav"
    />
  );
};

export default MusicPlayer;
