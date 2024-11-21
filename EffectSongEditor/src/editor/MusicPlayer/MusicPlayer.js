import { useRef, useState, useEffect } from "react";
import Player from './react-material-music-player/dist/components/Player'
import { Track, PlayerInterface } from './react-material-music-player/dist/index';
import ThemeProvider from "@mui/material/styles/ThemeProvider";
import { createTheme } from '@mui/material/styles';
import { green, purple } from '@mui/material/colors';

const MusicPlayer = ({ module }) => {
  const currentTime = useRef(0.0);
  const onStateChanged= (e) =>{
    console.log(e);
  }
  useEffect(() => {
    let tracks = [new Track(
      "test", null, 'test', 'test', 'test.wav'
    )];
    PlayerInterface.setPlaylist(tracks);
    PlayerInterface.subscribe(onStateChanged);
  }, [])

  useEffect(() => {
    const interval = setInterval(() => {
      if (module) {
        const state = PlayerInterface.getState();
        module.loop(state.currentTime, state.mediaState === "PLAYING");
      }
    }, 1000.0 / 60.0);
  }, [module]);

  const theme = createTheme({
    palette: {
      primary: {
        main: purple[500],
      },
      secondary: {
        main: green[500],
      },
      background: {
        paper: 'black'
      },
      action: {
        active: "#fff",
        hover: "rgba(255, 255, 255, 0.08)",
        hoverOpacity: 0.08,
        selected: "rgba(255, 255, 255, 0.16)",
        selectedOpacity: 0.16,
      },
      text: {
        disabled: "rgba(255, 255, 255, 0.5)",
        icon: "rgba(255, 255, 255, 0.5)",
        primary: "#fff",
        secondary: "#AAB4BE",
      },
    },
  });
  return (
    <ThemeProvider theme={theme}>
      <Player
        sx={{
          width: "100%",
          height: "100%",
          position: "static",
          boxSizing: "border-box",
          overflow: "hidden"
        }}
      />
    </ThemeProvider>
  );
};

export default MusicPlayer;