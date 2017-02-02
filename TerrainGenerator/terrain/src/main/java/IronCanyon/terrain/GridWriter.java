package IronCanyon.terrain;

import java.io.File;
import java.io.PrintWriter;

public class GridWriter {
   public static void write(String file, Terrain terrain, int xpoints, int ypoints) throws Exception {
      PrintWriter writer = new PrintWriter(new File(file));
      writer.println(terrain.getMinX() + " " + terrain.getMaxX() + " " + terrain.getMinY() + " " + terrain.getMaxY());
      writer.println(xpoints + " " + ypoints);
      double minx = terrain.getMinX();
      double maxx = terrain.getMaxX();
      double miny = terrain.getMinY();
      double maxy = terrain.getMaxY();
      for (int y = 0; y <= ypoints - 1; y++) {
         for (int x = 0; x <= xpoints - 1; x++) {
            double xval = ((xpoints - x - 1) * minx + x * maxx) / (xpoints - 1);
            double yval = ((ypoints - y - 1) * miny + y * maxy) / (ypoints - 1);
            if (terrain.isPassable(xval, yval)) {
               writer.print(String.format("%.4f", terrain.sample(xval, yval)));
            } else {
               writer.print("*");
            }
            if (x < xpoints) {
               writer.print(" ");
            }
         }
         writer.println();
      }


      writer.flush();
      writer.close();
   }
}
