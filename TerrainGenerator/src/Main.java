
public class Main {
   public static void main(String[] args) throws Exception {
      Terrain t = new Terrain();
      TerrainWriter.write("../resources/terrain.obj", t, 40, 40);
      GridWriter.write("../resources/terrain.grid", t, 40, 40);
      System.out.println("Wrote terrain files");
   }
}
