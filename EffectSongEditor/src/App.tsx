import './App.css';
import { ThemeProvider, createTheme } from '@mui/material/styles';
import Editor from './editor/Editor'

const theme = createTheme({
  palette: {
    primary: {
      main: '#868686',
    },
  },
});

function App() {
  return (
    <ThemeProvider theme={theme}>
      <Editor />
    </ThemeProvider>
  );
}

export default App;
