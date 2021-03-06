#ifndef MidiCurvePLUGINFILTER_H
#define MidiCurvePLUGINFILTER_H
#include "../../common/PizAudioProcessor.h"

#define MAX_ENVELOPE_POINTS    (32)
#define MAX_ENVELOPE_LENGTH    (127.0f)
#define MAX_ENVELOPE_GAIN      (127.0f)
#define MAX_ENVELOPE_DOT_SIZE  (8)
#define midiScaler (0.007874015748031496062992125984252)

enum parameters {
    kNumPointParams=MAX_ENVELOPE_POINTS*2, //x1,y1,x2,y2...
	kActive=kNumPointParams,//active1,active2,...
	kChannel=kActive+MAX_ENVELOPE_POINTS,
	kPitchBend,
	kPBRange,
	kPBRange2,
    kNumParams
};

class JuceProgram {	
friend class MidiCurve;
public:
	JuceProgram ();
	~JuceProgram () {}
private:
    float param[kNumParams];

    int lastUIWidth, lastUIHeight;
	String name;
};


//==============================================================================
/**
    A simple plugin filter that just applies a gain change to the audio
    passing through it.

*/
class MidiCurve  : public PizAudioProcessor,
                   public ChangeBroadcaster
{
public:
    //==============================================================================
    MidiCurve();
    ~MidiCurve();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

	void processBlock (AudioSampleBuffer& buffer,
                                     MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();

    //==============================================================================
#include "JucePluginCharacteristics.h"
    const String getName() const {return JucePlugin_Name;}
	bool hasEditor() const {return true;}
    bool acceptsMidi() const {
#if JucePlugin_WantsMidiInput 
        return true;
#else   
        return false;
#endif
    }
    bool producesMidi() const {
#if JucePlugin_ProducesMidiOutput
        return true;
#else 
        return false;
#endif
    }

    int getNumParameters()    { return kNumParams; }

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;


    //==============================================================================

    int getNumPrograms()      { return 128; }
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getCurrentProgramStateInformation (MemoryBlock& destData);
    void setCurrentProgramStateInformation (const void* data, int sizeInBytes);
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);


    //==============================================================================
    // These properties are public so that our editor component can access them
    //  - a bit of a hacky way to do it, but it's only a demo!

    // these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    int lastUIWidth, lastUIHeight;
	int lastCCOut, lastCCIn;
	float getPointValue(int n, int y);
	bool isPointActive(int point);
	bool isPointControl(int point);
	int getPrevActivePoint(int currentPoint);
	int getNextActivePoint(int currentPoint);
	void resetPoints();
	Path path;

    //==============================================================================
    juce_UseDebuggingNewOperator



private:
    float param[kNumParams];

	class midiPoint {
	public:
		midiPoint(float x, float y, bool active, bool control)
		{
			p.setXY(x,y);
			isControl=control;
			isActive=active;
		}
		midiPoint() 
		{
			p.setXY(0.f,0.f);
			isControl=false;
			isActive=true;
		}
		~midiPoint() {};

		Point<float> p;
		bool isControl;
		bool isActive;
	};

	struct PointComparator {
		int compareElements(midiPoint a, midiPoint b) { return roundFloatToInt(a.p.getX()*127.f) - roundFloatToInt(b.p.getX()*127.f); }
	} pointComparator;

	Array<midiPoint> points;
	void updatePath();

    JuceProgram *programs;
    int curProgram;
    
    bool init;

	float findValue(float input);
	double linearInterpolate(double x,double y1,double y2,double x1,double x2);
};



#endif
