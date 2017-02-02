package IronCanyon.terrain;

import com.flowpowered.noise.Noise;
import com.flowpowered.noise.NoiseQuality;

public class Terrain {
   
   public int getMinX() {
      return -200;
   }
   
   public int getMaxX() {
      return 200;
   }
   
   public int getMinY() {
      return -200;
   }
   
   public int getMaxY() {
      return 200;
   }
   
   public double sample(double x, double y) {
      double noiseHeight = Noise.gradientCoherentNoise3D(x / 50, y / 50, 0, 0, NoiseQuality.BEST) * -20;
      double edgeHeight = 0;
      if (Math.abs(x) > 190 || Math.abs(y) > 190) {
         edgeHeight = Math.max(Math.abs(x), Math.abs(y)) - 190;
         noiseHeight = noiseHeight / (1 + edgeHeight);
      }
      return edgeHeight + noiseHeight;
   }
   
   public boolean isPassable(double x, double y) {
      return Math.abs(x) < 191 && Math.abs(y) < 191;
   }
}
