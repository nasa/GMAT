import java.io.File;
import java.net.URISyntaxException;
import java.util.ArrayList;
import gmat.FileManager;

public class gmat_matlab_loadlibrary{
  public static void load(String lib){
    System.load(lib);
  }

  public static void loadLibrary(String[] libs){
    loadLibrary(libs, false);
  }

  public static void loadLibrary(String[] libs, boolean debug){
    loadLibrary(libs, debug, "../plugins");
  }

  public static void loadLibrary(String[] libs, boolean debug, String pluginDir){

    String os = System.getProperty("os.name");
    boolean isWin = os.indexOf("Windows") == 0;
    File path;
    File pluginPath;

    try{
      path = new File(gmat_matlab_loadlibrary.class.getProtectionDomain().getCodeSource().getLocation().toURI());
      path = path.getParentFile();
      pluginPath = new File(path, pluginDir);
    }
    catch (URISyntaxException e){
      System.err.println("ERROR: Unable to get valid path");
      return;
    }

    for (int ii=0; ii < libs.length; ii++){

      if (isWin){
        ArrayList<File> depLibFiles = new ArrayList<File>();

        switch (libs[ii].toLowerCase()) {
          case "gmat":
            if (debug) {
                depLibFiles.add(new File(path, "libGmatUtild.dll"));
                depLibFiles.add(new File(path, "libGmatBased.dll"));
            }
            else {
                depLibFiles.add(new File(path, "libGmatUtil.dll"));
                depLibFiles.add(new File(path, "libGmatBase.dll"));
            }
            break;
          case "station":
            if (debug) {
                depLibFiles.add(new File(pluginPath, "libStationd.dll"));
            }
            else {
                depLibFiles.add(new File(pluginPath, "libStation.dll"));
            }
            break;
          case "navigation":
            if (debug) {
              depLibFiles.add(new File(pluginPath, "libGmatEstimationd.dll"));
            }
            else {
              depLibFiles.add(new File(pluginPath, "libGmatEstimation.dll"));
            }
            break;
        }

        for (File depLibFile : depLibFiles) {
          System.load(depLibFile.getAbsolutePath());
        }
      }

      String suffix = "_java";
      if (debug) {
        suffix = suffix + "d";
      }

      File libFile = new File(path, System.mapLibraryName(libs[ii] + suffix));
      System.load(libFile.getAbsolutePath());
    }
  }

  public static void setGMATPath(){
    try{
      File path = new File(gmat_matlab_loadlibrary.class.getProtectionDomain().getCodeSource().getLocation().toURI());
      path = path.getParentFile();
      String pathStr = path.getAbsolutePath() + File.separator;

      FileManager fileManager = FileManager.Instance();
      fileManager.SetBinDirectory("gmat.jar", pathStr);
    }
    catch (URISyntaxException e){
      System.err.println("ERROR: Unable to get valid path");
      return;
    }
  }

  public static void loadGMAT(){
    loadGMAT(false);
  }

  public static void loadGMAT(boolean debug){
    loadGMAT(debug, "../plugins");
  }

  public static void loadGMAT(boolean debug, String pluginDir){
    String[] libs = {"gmat", "station", "navigation"};
    loadLibrary(libs, debug, pluginDir);
    setGMATPath();
  }
}
