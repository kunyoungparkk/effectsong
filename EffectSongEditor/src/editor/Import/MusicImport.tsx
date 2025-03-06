import { useState } from "react";
import { Button, Modal, Box } from "@mui/material";
import CloudUploadIcon from "@mui/icons-material/CloudUpload";
import FileUpload from "./FileUpload";

const MusicImport = ({ module, audioPlayerRef, notify, setLoading }: any) => {
  const [modalOpen, setModalOpen] = useState(false);

  const procMusicInput = (file: any) => {
    setLoading(true);
    setModalOpen(false);
    let reader = new FileReader();
    const MUSIC_ROOT_PATH = module.getRootPath() + "res/music/";
    if (!module.FS.analyzePath(MUSIC_ROOT_PATH).exists) {
        module.FS.mkdir(MUSIC_ROOT_PATH);
    }

    reader.onload = (e: any) => {
      let arrayBuffer = e.target.result;
      let filePath = MUSIC_ROOT_PATH + file.name;
      module.FS.writeFile(filePath, new Uint8Array(arrayBuffer));
      module.Renderer.getInstance().setAudioFile(filePath);
      module.FS.unlink(filePath);
      //audio 변경
      const blob = new Blob([arrayBuffer], { type: file.type });
      const url = URL.createObjectURL(blob);
      const prevURL = audioPlayerRef.current.audio.current.src;
      audioPlayerRef.current.audio.current.src = url;
      URL.revokeObjectURL(prevURL);

      setLoading(false);
      notify(file.name + " loaded successfully ", true);
    };
    reader.readAsArrayBuffer(file);
  };

  return (
    <div>
      <Button
        size="small"
        variant="contained"
        startIcon={<CloudUploadIcon />}
        sx={{ width: "100%" }}
        onClick={() => {
          setModalOpen(true);
        }}
      >
        music
      </Button>
      <Modal
        open={modalOpen}
        onClose={() => {
          setModalOpen(false);
        }}
        aria-labelledby="music-import-modal"
        aria-describedby="music-import-description"
      >
        <Box
          sx={{
            position: "absolute",
            top: "50%",
            left: "50%",
            transform: "translate(-50%, -50%)",
            width: "50%",
            height: "50%",
            bgcolor: "background.paper",
            border: "2px solid #000",
            boxShadow: 24,
            p: 4,
          }}
        >
          <FileUpload
            width="100%"
            height="100%"
            accept="audio/mp3, audio/wav, audio/flac"
            onChange={(e: any) => {
              procMusicInput(e.target?.files[0]);
            }}
            onDrop={(e: any) => {
              let files = e.dataTransfer?.files;
              if (files.length > 1) {
                notify("please upload single audio file");
                return;
              } else if (
                files[0].type !== "audio/flac" &&
                files[0].type !== "audio/wav" &&
                files[0].type !== "audio/mpeg"
              ) {
                notify("Audio file type mismatch. Only MP3, WAV, and FLAC are supported.");
                return;
              }
              procMusicInput(files[0]);
            }}
            hoverLabel="Click or drag to upload file (*.mp3, *.wav, *.flac)"
            dropLabel="Drop file here (*.mp3, *.wav, *.flac)"
          />
        </Box>
      </Modal>
    </div>
  );
};
export default MusicImport;
