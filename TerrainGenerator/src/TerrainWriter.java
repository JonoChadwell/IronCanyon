import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

public class TerrainWriter {

   private static class Triangle {
      public int vertex1;
      public int vertex2;
      public int vertex3;

      public Vector normal;

      public Triangle(int vertex1, int vertex2, int vertex3, List<Vertex> verticies) {
         this.vertex1 = vertex1;
         this.vertex2 = vertex2;
         this.vertex3 = vertex3;
         Vector a = verticies.get(vertex1 - 1).toVector();
         Vector b = verticies.get(vertex2 - 1).toVector();
         Vector c = verticies.get(vertex3 - 1).toVector();
         Vector ab = Vector.difference(a, b);
         Vector ac = Vector.difference(a, c);
         normal = Vector.unit(Vector.cross(ab, ac));
      }

      public String toString() {
         return "f " + vertex1 + "//" + vertex1 + " " + vertex2 + "//" + vertex2 + " " + vertex3 + "//" + vertex3;
      }

      public void addNormals(List<Vertex> verticies) {
         Vertex a = verticies.get(vertex1 - 1);
         Vertex b = verticies.get(vertex2 - 1);
         Vertex c = verticies.get(vertex3 - 1);
         a.normal = Vector.add(a.normal, normal);
         b.normal = Vector.add(b.normal, normal);
         c.normal = Vector.add(c.normal, normal);
      }
   }

   private static class Vertex {
      public double x;
      public double y;
      public double z;
      public Vector normal;

      public Vertex(double x, double y, double z) {
         this.x = x;
         this.y = y;
         this.z = z;
         normal = new Vector();
      }

//      public Vertex(Vector v) {
//         this.x = v.x;
//         this.y = v.y;
//         this.z = v.z;
//      }

      public String toString() {
         return "v " + x + " " + y + " " + z;
      }
      
      public String printNormal() {
         return "vn " + normal.x + " " + normal.y + " " + normal.z;
      }

      public Vector toVector() {
         return new Vector(x, y, z);
      }
   }

   public static void write(String file, Terrain terrain, int xpoints, int ypoints) throws Exception {
      List<Vertex> verticies = new ArrayList<>();
      List<Triangle> faces = new ArrayList<>();
      int[][] pts = new int[xpoints][ypoints];
      double minx = terrain.getMinX();
      double maxx = terrain.getMaxX();
      double miny = terrain.getMinY();
      double maxy = terrain.getMaxY();
      for (int x = 0; x <= xpoints - 1; x++) {
         for (int y = 0; y <= ypoints - 1; y++) {
            double xval = (x * minx + (xpoints - x - 1) * maxx) / (xpoints - 1);
            double yval = (y * miny + (ypoints - y - 1) * maxy) / (ypoints - 1);
            double zval = terrain.sample(xval, yval);
            Vertex vert = new Vertex(xval, zval, yval);
            verticies.add(vert);
            pts[x][y] = verticies.size();
         }
      }
      for (int x = 0; x < xpoints - 1; x++) {
         for (int y = 0; y < ypoints - 1; y++) {
            if (Math.random() < 0.5) {
               faces.add(new Triangle(pts[x][y], pts[x + 1][y], pts[x + 1][y + 1], verticies));
               faces.add(new Triangle(pts[x][y], pts[x + 1][y + 1], pts[x][y + 1], verticies));
            } else {
               faces.add(new Triangle(pts[x][y], pts[x + 1][y], pts[x][y + 1], verticies));
               faces.add(new Triangle(pts[x + 1][y], pts[x + 1][y + 1], pts[x][y + 1], verticies));
            }
         }
      }
      for (Triangle t : faces) {
         t.addNormals(verticies);
      }
      for (Vertex v : verticies) {
         v.normal = Vector.unit(v.normal);
      }

      PrintWriter writer = new PrintWriter(new File(file));
      for (Vertex v : verticies) {
         writer.println(v);
      }
      for (Vertex v : verticies) {
         writer.println(v.printNormal());
      }
      writer.println("s off");
      for (Triangle t : faces) {
         writer.println(t);
      }
      writer.flush();
      writer.close();
   }

   public static void main(String[] args) throws Exception {
      write("../resources/terrain.obj", new Terrain(), 40, 40);
   }

}
