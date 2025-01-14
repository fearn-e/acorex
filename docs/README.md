# ACorEx

Thanks for downloading ACorEx!

# Instructions for Use

## Known Issues
* The "Bigger UI" button will reset the program and close any open corpus.
* The software will freeze when analysing. While on Windows, progress is reported in the terminal window, on MacOS, there is currently no progress reporting. For an analysis of 2000 small drum one shots with the default settings, it may take around up to a minute to complete. Reductions will take exponentially longer with bigger corpora.

## Analysis

Before exploring a corpus in Explorer view, you must first analyse some audio files.

* Select Analyse and Analyse Corpus
* Pick an audio directory and output file
* At this point, you may tweak any desired settings
* Click confirm and wait!

You can also **insert into existing analyses** by selecting an existing corpus file as your output file, or **reduce the dimensionality** of an already existing corpus file by selecting Reduce Corpus.

## Exploring

Once you have analysed a set of audio files, you may open them in Explore view.

* Select Explore and click Open Corpus to select your analysed corpus file
* Switch between space and colour dimensions with the dropdowns on the right to pick your various descriptors
* By setting one of the 3 space dimensions to None, you can drop into a 2D view

##### Note: If no audio can be heard or if audio is glitchy, try changing the output device selected - multiple devices with the same name will be for different drivers. If audio is still glitchy, it may be due to the corpus being too large.

### Controls

Navigation can be done with a combination of mouse and keyboard controls.

#### Navigation:
* W-A-S-D-R-F - move the camera in 3D space
* Q-E - rotate the camera left and right
* Z-X - zoom in and out
* Left click drag - rotate the camera
* Middle mouse drag - pan the camera
* Middle mouse scroll - zoom in and out

#### Interaction:
* Right click - select the nearest point / trail
* SPACE - create playhead on currently selected file
* click the X on the playhead blocks at the bottom of the screen to delete playheads
* adjust various playback settings in the Explore sub-menu on the right

#### Misc:
* ENTER - open the currently selected file in the OS file explorer
* C - copy the currently selected file path to clipboard
