package IronCanyon.terrain;

public class Main {
   public static void main(String[] args) throws Exception {
      Terrain t = new Terrain();
      TerrainWriter.write("../../resources/terrain.obj", t, 200, 200);
      GridWriter.write("../../resources/terrain.grid", t, 200, 200);
      System.out.println("Wrote terrain files");
   }
}
