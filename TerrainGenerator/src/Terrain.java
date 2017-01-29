
public class Terrain {
   
   public int getMinX() {
      return -10;
   }
   
   public int getMaxX() {
      return 10;
   }
   
   public int getMinY() {
      return -10;
   }
   
   public int getMaxY() {
      return 10;
   }
   
   public double sample(double x, double y) {
      return Math.min(Math.max(0, Math.sqrt(Math.abs(x * y)) * 2 - 8), 3.0);
   }
   
   public static void main(String[] args) {
      Terrain t = new Terrain();
      System.out.println(t.sample(10,0));
   }
}
