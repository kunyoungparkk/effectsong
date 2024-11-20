import React, { useRef, useState, useEffect } from "react";

const MusicPlayer = ({setMusicTime}) => {
  const audioRef = useRef(null); // audio 태그 참조
  const [isPlaying, setIsPlaying] = useState(false); // 재생 상태
  const [currentTime, setCurrentTime] = useState(0); // 현재 재생 시간
  const [duration, setDuration] = useState(0); // 총 재생 시간

  useEffect(() => {
    const interval = setInterval(() => {
      if (audioRef.current) {
        setMusicTime(audioRef.current.currentTime);
      }
    }, 1000/60);
    }, []);
  
    const handlePlayPause = () => {
    if (isPlaying) {
      audioRef.current.pause();
    } else {
      audioRef.current.play();
    }
    setIsPlaying(!isPlaying);
  };

  const handleTimeUpdate = () => {
    setCurrentTime(audioRef.current.currentTime);
  };

  const handleLoadedMetadata = () => {
    setDuration(audioRef.current.duration);
  };

  const handleSeek = (e) => {
    const seekTime = (e.target.value / 100) * duration;
    audioRef.current.currentTime = seekTime;
    setCurrentTime(seekTime);
  };

  return (
    <div>
      <audio
        ref={audioRef}
        src="debugremaster.wav"
        onTimeUpdate={handleTimeUpdate}
        onLoadedMetadata={handleLoadedMetadata}
      ></audio>
        <button onClick={handlePlayPause}>
          {isPlaying ? "Pause" : "Play"}
        </button>
        <input
          type="range"
          value={(currentTime / duration) * 100 || 0}
          onChange={handleSeek}
        />
        <span>{Math.floor(currentTime)}s / {Math.floor(duration)}s</span>
    </div>
  );
};

export default MusicPlayer;