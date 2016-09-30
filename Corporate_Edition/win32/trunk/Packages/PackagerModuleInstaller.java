/*
 * Created by IntelliJ IDEA.
 * User: Rob Turner
 * Date: Feb 26, 2002
 * Time: 6:59:31 AM
 * To change template for new class use
 * Code Style | Class Templates options (Tools | IDE Options).
 */

import java.io.File;
import java.util.Iterator;
import java.util.TreeSet;
import java.util.Vector;
import java.lang.Thread;

public class PackagerModuleInstaller {

	long totalFileCount = 0;
	long totalMatches = 0;
	long totalDirectories = 0;
    int largestBuildNumberFound = 0;
	TreeSet foundItems = new TreeSet();

	public static void main(String args[]) {
		try {
            String basePathForProductModules = args[0];
            String basePathForPackager = args[1];
			PackagerModuleInstaller installer = new PackagerModuleInstaller(basePathForProductModules, basePathForPackager);
		} catch (Throwable e) {
			e.printStackTrace(System.out);
			System.out.println("Major Failure.  Sorry...");
		}
	}

	/**
	 *	Uninstalls the currently install Packager.  Note: Routine will not fail if Packager is not installed.
	 */
	public void UninstallProduct(String GUID, String productName) {

		// Uninstall using Packager GUID.

		String command = "MSIExec.exe /x" + GUID + " /qn";

		Runtime aRuntime = Runtime.getRuntime();
		try {
			System.out.println("Uninstalling " + productName);
			Process theTast = aRuntime.exec(command);
			theTast.waitFor();
		} catch (Throwable error) {
			System.out.println("Execution Failed");
		}
	}

	/**
	 *	Installs the latest Packager.
	 *
	 *	@param root directory shared by modules and application
	 */
	public void InstallPackager(String rootDir) {
		// Create the path to the MSI file for the packager and then execute the installer.

		File directoryContainingPackagerInstaller = new File(rootDir);
		File packagerInstallerFile = new File(directoryContainingPackagerInstaller, "Symantec Packager.msi");

		// NOTE:  	We would like to use the argument '/qn' to make the installation of the Packager
		//			Silent.  However, the Packager Installer does not seem to handle silent installs.
		//			The good news is that all the product modules do install without interaction.

		String command = "MSIExec /i \"" + packagerInstallerFile.getAbsolutePath() + "\" /qn";
		try {
			Runtime aRuntime = Runtime.getRuntime();
			System.out.println("Installing Packager. Path: \"" + packagerInstallerFile.getAbsolutePath() + "\"");
			Process theTast = aRuntime.exec(command);
			theTast.waitFor();
		} catch (Throwable error) {
			System.out.println("Execution Failed");
		}
	}

	/**
	 *	Installs the latest Product Modules.
	 *
	 *	@param root directory shared by modules and Packager
	 */
	public void InstallProductModules(String rootDir) {
		String productModuleDirectory = ((new File(rootDir)).getAbsolutePath());

		Vector filesToInstall = new Vector(20);

		// Out of the product modules found only install those that are in the
		// latest build directory.

		for (Iterator iter = foundItems.iterator(); iter.hasNext();) {
			String onePath = new String((String) iter.next());
			if (onePath.startsWith(productModuleDirectory)) {
				filesToInstall.addElement(onePath);
			}
		}

		System.out.println("Installing Product Modules...\n");

		Runtime aRuntime = Runtime.getRuntime();
		String[] arguments = new String[2];

		String commandPrefix = "MSIExec.exe /i ";

		for (int i = 0; i < filesToInstall.size(); i++) {
			String command = commandPrefix;
			command += " \"" + (String) filesToInstall.elementAt(i) + "\"";
			try {
				File getFileName = new File((String) filesToInstall.elementAt(i));
				System.out.println("Installing " + "\"" + getFileName.getPath() + "\"...");
				Process theTast = aRuntime.exec(command);
				theTast.waitFor();
				java.lang.Thread.sleep(10000);
			} catch (Throwable error) {
				System.out.println("Execution Failed");
			}
		}
	}

	/**
	 *	The constructor attempts the following:
	 *
	 *  	1) Locate the most current build.
	 * 		2) Uninstall the current Packager.
	 * 		3) Install the latest Packager.
	 * 		4) Install all found product modules.
	 *
	 *	@param directory Starting loocation for a search.
	 *  @param pattern Files that match the pattern are saved.
	 */
	public PackagerModuleInstaller(String productModuleSearchDirectory, String packagerSearchDirectory ) {

        LocateAndInstallPackager(packagerSearchDirectory);
        LocateAndInstallProductModules(productModuleSearchDirectory);

	}

    private void LocateAndInstallPackager(String packagerSearchDirectory) {

        foundItems = new TreeSet();

        System.out.print("\nLocating Most Recent Build of Packager ");

          // Scan the directory to locate the most current build.
          // The starting point is specified by the user.

          File directory = new File(packagerSearchDirectory);

          // Recursive search locates files that match the pattern specified.

          RecursiveSearch(directory, "Symantec Packager.msi");

          System.out.println();

          if (foundItems.size() > 0) {
 			String buildRootDir = FindLatestBuild(foundItems);
             if ( largestBuildNumberFound != 0 )
            {
                System.out.println("Installing Packager from build " + largestBuildNumberFound );
            }
            else
            {
                System.out.println("Installing Packager from " + buildRootDir);
            }
            UninstallProduct("{D8A4D550-6301-449F-B7A3-7413F4180F80}", "Packager");
            InstallPackager(buildRootDir);
          } else {
			System.out.println("Packager Not Found.");
          }


    }

    private void LocateAndInstallProductModules(String productModuleSearchDirectory) {
        foundItems = new TreeSet();

        System.out.print("\nLocating Most Recent Build of Product Modules ");

		// Scan the directory to locate the most current build.
		// The starting point is specified by the user.

		File directory = new File(productModuleSearchDirectory);

		// Recursive search locates files that match the pattern specified.

		RecursiveSearch(directory, ".pmi");

		System.out.println();

        if (foundItems.size() > 0) {

			// Find the directory with the latest product modules.
			// buildRoot will be something like: "x:\qa\Mercury\builds\r1.0\PACKAGER.86\PACKAGER\CD"

			String buildRootDir = FindLatestBuild(foundItems);
            File usedToGetParent = new File(buildRootDir);
            buildRootDir = usedToGetParent.getParent();

            if ( largestBuildNumberFound != 0 )
            {
                System.out.println("Installing Product Modules from build " + largestBuildNumberFound );
            }
            else
            {
                System.out.println("Installing Product Modules from " + buildRootDir);
            }

			// Remove old and install new...

			//UninstallProduct("{9178DC44-2611-4041-B70C-AC7837BC8D31}", "\"IE5.5\"");
			//UninstallProduct("{A09CBBA9-4C48-45E0-8692-66C09C480536}", "\"LiveUpdate\"");
			//UninstallProduct("{1B606B33-BCC1-46C0-B7D6-78D9DF4022D1}", "\"LiveUpdate Admin\"");
			//UninstallProduct("{1D8234AB-1E1D-4700-B70A-A2C328009BF7}", "\"Microsoft Runtime Library\"");
			//UninstallProduct("{08B0627E-487F-4891-A83F-BCC015928024}", "\"MMC\"");
			//UninstallProduct("{DA108165-72C1-4BE6-BE83-FD128C2C8BD2}", "\"NTDeploy\"");
			//UninstallProduct("{ED131273-02FF-4D35-B6B3-5D1A34537EF3}", "\"SESA Collector\"");
			//UninstallProduct("{15318592-097E-4F1A-8D30-AC10570C8705}", "\"Symantec Client Firewall Adming\"");
			//UninstallProduct("{5F5A0389-C8D6-49D6-8812-9F7F439E72AC}", "\"Symantec System Center\"");
			//UninstallProduct("{AE5A0D80-4DEB-4B06-9B41-D09D54356D80}", "\"SymEvent\"");
			//UninstallProduct("{87686516-8CAD-4778-B3E6-B02FF6252202}", "\"WebInstall\"");
			//UninstallProduct("{04270AE2-594A-4C69-9806-88BCEBFAA3D9}", "\"Central Quarantine\"");
			//UninstallProduct("{8F00B75C-3F13-49F2-9D8C-9C52E570F4F0}", "\"Quarantine Console\"");
			UninstallProduct("{1D54BDAC-C362-4849-B52F-814319CF5057}", "\"Symantec Client Firewall\"");
			UninstallProduct("{BD12EB47-DBDF-11D3-BEEA-00A0CC272509}", "\"SAV Client\"");
			UninstallProduct("{7D2B86CA-2D5D-469E-92ED-E56B62BD1D3C}", "\"SAV Server\"");

			InstallProductModules(buildRootDir);
		} else {
			System.out.println("No Files Found After Examining " + totalFileCount +
					" Files In " + totalDirectories + " Directories.");
		}

    }

    /**
	 * Finds the latest Build Root.
	 *
	 * @param list The list of files to scan.
	 */
	public String FindLatestBuild(TreeSet list) {

		// Find the largest build number in the path by pulling out the string
		// representation of the number, converting it to an integer, and
		// comparing it to the largest seen so far.

		largestBuildNumberFound = 0;
		String pathWithLargestBuildNumber = (String) list.last();

		Iterator iter = list.iterator();

		while (iter.hasNext()) {
			String pathname = (String) iter.next();
            File currentFile = new File(pathname);

            // Walk the pathname starting from the rear.  We are looking for
            // the first number in the pathname.  We assume that the build number
            // is in the directory name closest to the leaf.  We do need to
            // special case the name "disk1" which is not a build number.

            while ( (currentFile = currentFile.getParentFile()) != null )
            {
                String directoryName = currentFile.getName();
                int number = -1;
                boolean numberFound = false;
                StringBuffer numberBuffer = new StringBuffer();
                if ( directoryName.equalsIgnoreCase("disk1") == false )
                {
                    for ( int i = 0; i < directoryName.length(); i++ )
                    {
                        char oneChar;
                        if ( (oneChar = directoryName.charAt(i)) >= '0' &&
                              oneChar <= '9' )
                        {
                            numberFound = true;

                            numberBuffer.append(oneChar);
                        } else {
                            if ( numberFound )
                            {
                                break;
                            }
                        }
                    }
                }
                // assume that the number is the build number
                if ( numberFound )
                {
                    try {
						int currentBuildNumber = Integer.parseInt(new String(numberBuffer));
						if (currentBuildNumber > largestBuildNumberFound) {
							largestBuildNumberFound = currentBuildNumber;
							pathWithLargestBuildNumber = pathname;
						}
					} catch (NumberFormatException e) {
					}
                     break;
                }
            }
 		}

		// Now that we know the largest build number let's find the root.
		if (pathWithLargestBuildNumber != null) {
			File lastFile = new File(pathWithLargestBuildNumber);
			String parentDirectory = lastFile.getParent();

			return parentDirectory;
		} else {
			System.out.println("Error: Unknown path.  Build number not found!");
			return null;
		}
	}

	/**
	 *	Recursively walks a directory structure starting at the 'directory' specified, and saves all
	 *  File entries who name ends in the pattern specified. Files that match the pattern are stored
	 *  in the class member 'foundItems'
	 *
	 *	@param directory Starting loocation for a search.
	 *  @param pattern Files that match the pattern are saved.
	 */
	public void RecursiveSearch(File directory, String pattern) {

		if (directory.isDirectory()) {
			totalDirectories++;
			File[] entries = directory.listFiles();

			// Iterate the contents of the directory.

			if (entries != null) {
				for (int i = 0; i < entries.length; i++) {
					// If it's a directory then dive into the directory.
					if (entries[i].isDirectory()) {
						RecursiveSearch(entries[i], pattern);
					} else {
						// Sss if file matches the pattern.
						try {
							boolean match = false;

							if (entries[i].length() > pattern.length()) {
								String filename = entries[i].getName();

								// Compare region ignoring case.

								if (filename.regionMatches(true,
										(int) (entries[i].getName().length() - pattern.length()),
										pattern,
										0,
										pattern.length())) {
									totalMatches++;
									match = true;
									foundItems.add(entries[i].getCanonicalPath().toLowerCase());
								}
							}
							if (match) {
								System.out.print("*");
							} else {
								System.out.print("-");
							}
						} catch (Throwable err) {
						}
						totalFileCount++;
					}
				}
			}
		}
	}
}
