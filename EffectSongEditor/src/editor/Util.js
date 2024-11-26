export default function useUtil() {
    return {
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
        roundToNearestStep: (value, step = 0.0001, epsilon = 1e-4) => {
            const nearest = Math.round(value / step) * step;
            return Math.abs(value - nearest) < epsilon ? nearest : value;
        }
    }
}