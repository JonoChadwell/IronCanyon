
public class Terrain {
   
   public int getMinX() {
      return -60;
   }
   
   public int getMaxX() {
      return 60;
   }
   
   public int getMinY() {
      return -60;
   }
   
   public int getMaxY() {
      return 60;
   }
   
   public double sample(double x, double y) {
      return Math.min(Math.max(Math.sqrt(x * x + y * y) / 30.0 - 1.0, Math.sqrt(Math.abs(x * y)) * 2 - 50), 15.0);
   }
   
   public boolean isPassable(double x, double y) {
      return sample(x,y) < 0.5;
   }
}
