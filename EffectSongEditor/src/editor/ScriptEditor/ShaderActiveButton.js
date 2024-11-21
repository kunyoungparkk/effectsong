import { Unstable_Popup as BasePopup } from '@mui/base/Unstable_Popup';
import { styled } from '@mui/system';
import { Button } from '@mui/material';

export default function ShaderActiveButton({ scriptVisible, setScriptVisible }) {
    return (
            <BasePopup open={true} placement='top-end' anchor={document.getElementById('engineDiv')} offset={-53}>
                <PopupBody>
                    <Button variant="contained" color="secondary" id="scriptVisible" size="medium"
                        onClick={(e) => {
                            setScriptVisible(!scriptVisible);
                        }}>shader on&off</Button>
                </PopupBody>
            </BasePopup>
    );
}

const PopupBody = styled('div')(
    ({ theme }) => `
  width: 170px;
  height: 50px;
  padding: 0px 0px 0px 0px;
  margin: 0px 0px 0px 0px;
  background-color: rgba(0,0,0,0);
  box-shadow: ${theme.palette.mode === 'dark'
            ? `0px 4px 8px rgb(0 0 0 / 0.7)`
            : `0px 4px 8px rgb(0 0 0 / 0.1)`
        };
  font-family: 'IBM Plex Sans', sans-serif;
  font-weight: 500;
  font-size: 0.875rem;
  z-index: 1;
`,
);
