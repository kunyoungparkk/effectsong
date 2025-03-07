const Util =  {
        cross: (A: number[], B: number[]) => {
            return [A[1] * B[2] - A[2] * B[1], A[2] * B[0] - A[0] * B[2], A[0] * B[1] - A[1] * B[0]];
        },
        normalize: (A: number[]) => {
            const length = Math.sqrt(A[0] * A[0] + A[1] * A[1] + A[2] * A[2]);
            return [A[0] / length, A[1] / length, A[2] / length];
        },
        blobToArrayBuffer: (blob: Blob) => {
            return new Promise((resolve, reject) => {
                const reader = new FileReader();
                reader.addEventListener('loadend', () => {
                    resolve(reader.result);
                });
                reader.addEventListener('error', reject);
                reader.readAsArrayBuffer(blob);
            });
        },
        isValidNum: (text: string) => {
            if (text === '-' || text === '' || isNaN(parseFloat(text))) {
                return false;
            } else {
                return true;
            }
        },
        roundToNearestStep: (value: number, step: number = 0.001, epsilon: number = 1e-7) => {
            const nearest = Math.round(value / step) * step;
            return Math.abs(value - nearest) < epsilon ? nearest : value;
        }
}
export default Util;