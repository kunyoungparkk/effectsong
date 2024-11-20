import Quaternion from 'quaternion';

export default function useUtil() {
    return {

        /*중간 layer 함수*/
        //오일러각을 쿼터니언으로 변환
        quatFromEuler: (x, y, z) => {
            const deg = Math.PI / 180;
            return Quaternion.fromEulerLogical(x * deg, y * deg, z * deg, "XYZ");
        },
        //쿼터니언 값을 오일러각으로 변환
        eulerFromQuat: (w, x, y, z) => {
            const quat = new Quaternion(w, x, y, z);
            const euler = quat.toEuler("XYZ");
            const toEuler = 180 / Math.PI;
            for (let i = 0; i < 3; i++) {
                euler[i] *= toEuler;
            }
            return euler;
        },
        //외적(cross product) 연산
        cross: (A, B) => {
            return [A[1] * B[2] - A[2] * B[1], A[2] * B[0] - A[0] * B[2], A[0] * B[1] - A[1] * B[0]];
        },
        normalize: (A) =>{
            const length = Math.sqrt(A[0]*A[0] + A[1]*A[1] + A[2]*A[2]);
            return [A[0]/length, A[1]/length, A[2]/length];
        },
        blobToArrayBuffer: (blob) => {
            return new Promise((resolve, reject) => {
                const reader = new FileReader();
                reader.addEventListener('loadend', () => {
                    resolve(reader.result);
                });
                reader.addEventListener('error', reject);
                reader.readAsArrayBuffer(blob);
            });
        },
        isValidNum : (text)=>{
            let parsedValue = parseFloat(text);
            if (!isNaN(parsedValue) || text === '-' || text === ''){
                return true;
            }else{
                return false;
            }
        },
        getValidNum: (text) =>{
            let parsedValue = parseFloat(text);
            if (!isNaN(parsedValue)) {
                return parsedValue;
            }else if(text === '-' || text === ''){
                return 0.0;
            }
        },
        getParsedNumber: (text)=>{
            return parseFloat(text.toFixed(7));
        }

        //이벤트리스너 및 addFunction으로 함수포인터 넘긴 것 정리
        //2D Scene에 대해선 적용안됨.. 추후에 이벤트 관련해서 뭔가 처리가 필요.
        //이렇게하면 3D Scene에 적용된것만 지워질것임..
        // const removeEventListeners = () => {
        //     let stage = module.Stage.getInstance(context.current);
        //     let scene = stage.getChildAt(0);
        //     stage.delete();
        //     if (!scene) {
        //         return;
        //     }
        //     const eventDataKeys = Object.keys(objEventData.current);
        //     for (let i = 0; i < eventDataKeys.length; i++) {
        //         const key = eventDataKeys[i];
        //         const eventObj = objEventData.current[key];
        //         if (eventObj.eventListeners.length > 0) {
        //             for (let j = 0; j < eventObj.eventListeners.length; j++) {
        //                 let curEventListener = eventObj.eventListeners[j];
        //                 scene.removeEventListener(curEventListener.evtType, curEventListener.eventListener);
        //                 module.removeFunction(curEventListener.function);
        //             }
        //             eventObj.eventListeners[0].eventListener.delete();
        //         }
        //         //오브젝트도 일단 지우기
        //         eventObj.obj.delete();
        //     }
        //     scene.delete();
        // }

    }

}