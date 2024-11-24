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
        normalize: (A) => {
            const length = Math.sqrt(A[0] * A[0] + A[1] * A[1] + A[2] * A[2]);
            return [A[0] / length, A[1] / length, A[2] / length];
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
        isValidNum: (text) => {
            if (text === '-' || text === '' || isNaN(parseFloat(text))) {
                return false;
            } else {
                return true;
            }
        },
        getParsedNumber: (text) => {
            return parseFloat(text.toFixed(7));
        },
        epsilonRound: (value, epsilon = 1e-7) => {
            return Math.abs(value - Math.round(value)) < epsilon
                ? Math.round(value)
                : value;
        },
        roundToNearestStep: (value, step = 0.000001, epsilon = 1e-7) => {
            const nearest = Math.round(value / step) * step;
            return Math.abs(value - nearest) < epsilon ? nearest : value;
        }
    }
}