# Implementation #

There are six tasks detailed in the next section that need to be completed for this project.  The descriptions are perhaps slightly optimistic, but we should try to get them done as fully as possible.  The breakdown into separate systems should be almost completely modularized, so that each piece can be developed and tested independently.  The only real overlap will be in designing the communication mechanism between pieces.

Since OpenCV is only currently available in C++, the tracking and recognition systems will need to be written in C++.  The testing and client applications could be written in any language, since they will hopefully be abstracted away from the particular implementation of the tracking/recognition systems.


# Tasks #

### Physical Construction ###

We need to order cameras and have them by the start of reading period.  We need a baseball cap or other hat with a stiff brim that can be dismantled.  The cameras need to be attached stably and securely to the hat.  We may need to wire a couple LEDs for lighting, depending on the video quality.


### Tracking Systems ###

Given an image of the eye area, there are several parts that we want to track.  First and foremost is the pupil location.  For reference, we should also know the eye boundary.  As a secondary goal, we would like to track eyebrow position and shape.

For this system, OpenCV vision algorithms will be the most useful.  Given the sharp contrasts in and around the eye, edge detection routines will probably be our best bet.  We probably can also do some basic color analysis since just about everybody has black pupils and white surrounding area, regardless of iris, skin or hair color.  This will not be the case for the eyebrows.  Finding the iris is essentially equivalent to finding the pupil, so techniques currently used in iris scanning (which is a well-developed field) should be applicable.

This system will take as input the images from two cameras, one for each eye, and will produce for each frame a struct/class containing the tracking information.  This could be as simple as coordinates for key points, but may also contain more complex data, such as the curvature of the eyebrow or the shape of the eye.

This step of the system will be time-independent.  It produces data for only a single frame of the video.  The algorithms may use past frames to improve tracking performance, but we do not perform any data-stream/signal analysis in this step.

### Recognition Systems ###

Given a stream of data about the eye (i.e. pupil location, eyebrow position, etc.) that is produced in the tracking stage, we want to extract "gestures".  Ideally the code would be flexible enough that we can easily add new gestures, but let's start by focusing on the following seven items:

  * Moving pupil from center to left
  * Moving pupil from center to right
  * Moving pupil from center to down
  * Moving pupil from center to up
  * Winking (left & right eye)
  * Extended blink / closed eyes
  * Eyebrow raise

Machine learning techniques will be important in this stage.  Basic thresholding may be able to get us started, but will likely have a high false positive rate.  Perhaps some sort of clustering or SVM over an n-th order Markov model would be more appropriate (i.e. treating the last n data points as a vector in n-space).  Since there is so much random movement around the eyes, we will want to focus on keeping the false positive rate down as much as possible.

This system will take as input the data produced in the tracking stage, and will asynchronously produce "gesture events" that trigger some sort of action.  The particular action taken will depend on the client application that is listening for gestures, and will not be included as part of the recognition system.

### Client Interaction ###

Once the recognition system decides that a gesture has occurred, we need a way to communicate that to whatever programs are taking input from the eye tracker.  The goal is that the eye tracker would function essentially as ubiquitous hardware like the mouse or keyboard, but depending on complexity we may have to have a more focused communication system.

Options for signaling a gesture are:

  * Callback functions, for testing/training/monitoring systems that would be linked to our code.
  * Custom OS-level signals/events.  This would require a different solution depending on the operating system, and probably would be the most complex.  Windows has an event-driven framework, so we would probably need to define custom events and trigger them as needed.
Linux doesn't have events, so we'll probably need to use pipes and signals.
  * Fake a keypress.  This is an intriguing idea, as it would allow us to use the eye-tracker with any existing application.  Basically we'd map each gesture to a different key (e.g. arrow keys for pupil movement, space for an eyebrow raise and enter for an extended blink), and then just programmatically trigger a keypress when the gesture is detected.
  * Network sockets.  Set up a client/server type interface on a port and send the signals that way.  This is an intriguing idea, as it would open the possibility of using the eye-tracker across a network, which could have a lot of cool applications.

This should be a library that exposes a simple API into the eye-tracker.  The extent and complexity of the library will be dependent on the particular method (or methods) selected.  A sample usage might be:
```
int handle = eyeTrackerConnect();
while (1)
{
  int gesture = waitForGesture(handle);
  switch(gesture)
  {
  case EYE_MOVE_LEFT:
    /* DO SOME COOL STUFF */
  case EYE_MOVE_RIGHT:
    /* DO SOME OTHER COOL STUFF */
  }
}
eyeTrackerRelease(handle);
```


### Testing Application ###

We need some quantitative testing results to demonstrate usability.  We could use a tapping task, but that isn't particularly amenable to gestures, and would probably just show that eye tracking sucks for continuous control (which we know already).  This should be a single application (with GUI) that offers a few kinds of tests:
  * Baseline: Tell user to act normally without making any deliberate eye motions.  Track the gestures that are produced (false positives), and produce statistics on which gestures were detected and how frequently.
  * Gestures on Cue: Tell user to perform a specific gesture, and then wait for first gesture detected (or set amount of time).  Repeat many times, and compute statistics such as accuracy, false negative rate, commonly confused gestures, etc.  This could also be extended to accommodate sequences of gestures.


### Client Application ###

Our main goal is really to produce a nice interface that client applications can use, so we shouldn't spend too much time creating our own client apps.  However, a simple demo for proof-of-concept purposes is a good idea.  Perhaps a typing application, which would demonstrate a use for accessibility.  Each letter would be mapped to a sequence of gestures.

We should also find some existing application and try to make it respond to our eye gestures.  Perhaps a simple game, like tetris or snake.