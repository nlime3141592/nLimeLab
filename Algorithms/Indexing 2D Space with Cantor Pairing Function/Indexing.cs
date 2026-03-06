using System;
using System.Diagnostics;

namespace nl
{
    public static class WorldUtil
    {
        public static int PositionToIndex(int x, int y)
        {
            Debug.Assert(x >= -8192 && x < 8192);
            Debug.Assert(y >= -8192 && y < 8192);

            if (x == 0 && y == 0)
            {
                return 0;
            }

            int q = 0;

            while (x <= 0 || y < 0)
            {
                int t = x;
                x = -y;
                y = t;
                ++q;
            }

            // p = CantorPairing(x', y); x' = x - 1;
            int xpy = x + y;
            int p = xpy * (xpy - 1) / 2 + y;
            return 4 * (p + 1) - q;
        }

        public static void IndexToPosition(out int chunkPositionX, out int chunkPositionY, int index)
        {
            Debug.Assert(index >= 0 && index <= 536911870);

            int px = 0;
            int py = 0;

            if (index == 0)
            {
                chunkPositionX = px;
                chunkPositionY = py;
                return;
            }

            int q = 4 - (index % 4);
            int p = (index + q % 4) / 4 - 1;

            int w = (int)Math.Floor((Math.Sqrt(8 * p + 1) - 1) * 0.5);
            int t = (w * w + w) / 2;

            // (x', y) = CantorPairingInverse(p); x' = x - 1;
            py = p - t;
            px = w - py + 1;

            while (q < 4)
            {
                t = px;
                px = -py;
                py = t;
                ++q;
            }

            Debug.Assert(px >= -8192 && px < 8192);
            Debug.Assert(py >= -8192 && py < 8192);

            chunkPositionX = px;
            chunkPositionY = py;
            return;
        }
    }
}