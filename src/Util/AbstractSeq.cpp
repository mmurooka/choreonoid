/**
   @file
   @author Shin'ichiro Nakaoka
*/

#include "AbstractSeq.h"
#include "YAMLWriter.h"
#include <boost/format.hpp>
#include "gettext.h"

using namespace std;
using namespace cnoid;

using boost::format;

namespace {

string dummySeqMessage;

}


AbstractSeq::AbstractSeq(const char* seqType)
    : seqType_(seqType)
{

}


AbstractSeq::AbstractSeq(const AbstractSeq& org)
    : seqType_(org.seqType_),
      contentName_(org.contentName_)
{

}


AbstractSeq& AbstractSeq::operator=(const AbstractSeq& rhs)
{
    seqType_ = rhs.seqType_;
    contentName_ = rhs.contentName_;
    return *this;
}


void AbstractSeq::copySeqProperties(const AbstractSeq& source)
{
    seqType_ = source.seqType_;
    contentName_ = source.contentName_;
}


AbstractSeq::~AbstractSeq()
{

}


double AbstractSeq::getTimeOfFrame(int frame) const
{
    return (frame + getOffsetTimeFrame()) / getFrameRate();
}


int AbstractSeq::getFrameOfTime(double time) const
{
    return static_cast<int>(time * getFrameRate()) - getOffsetTimeFrame();
}


int AbstractSeq::getOffsetTimeFrame() const
{
    return 0;
}


bool AbstractSeq::setOffsetTimeFrame(int offset)
{
    return (offset == 0);
}


bool AbstractSeq::readSeq(const Mapping& archive, std::ostream& os)
{
    try {
        if(contentName_.empty()){
            if(!archive.read("content", contentName_)){
                archive.read("purpose", contentName_); // old version
            }
        }
        auto frameRateNode = archive.find("frameRate");
        if(frameRateNode->isValid()){
            if(frameRateNode->isString() && frameRateNode->toString() == "irregular"){
                frameRateNode->throwException(_("Irregular interval data cannot be loaded."));
            }
            setFrameRate(frameRateNode->toDouble());
        }

        return doReadSeq(archive, os);

    } catch (ValueNode::Exception& ex) {
        os << ex.message();
        return false;
    }
}


bool AbstractSeq::doReadSeq(const Mapping&, std::ostream&)
{
    return true;
}


bool AbstractSeq::checkSeqContent(const Mapping& archive, const std::string requiredContent, std::ostream& os)
{
    string content_;
    if(!archive.read("content", contentName_)){
        archive.read("purpose", contentName_); // old version
    }
    if(contentName_ == requiredContent){
        return true;
    }

    if(content_.empty()){
        os << format(_("Content of %1% should be \"%2%\" but it is not specified."))
            % seqType() % requiredContent;
    } else {
        os << format(_("Content \"%1%\" of %2% is different from the required content \"%3%\"."))
            % contentName_ % seqType() % requiredContent;
    }
    return false;
}


bool AbstractSeq::writeSeq(YAMLWriter& writer)
{
    if(seqType_.empty()){
        if(contentName_.empty()){
            writer.putMessage(_("The type of the sequence to write is unknown."));
        } else {
            writer.putMessage(str(format(_("The type of the %1% sequence to write is unknown.")) % contentName_));
        }
        return false;
    }

    writer.startMapping();
        
    writer.putKeyValue("type", seqType());
    if(!contentName_.empty()){
        writer.putKeyValue("content", contentName_);
    }
    writer.putKeyValue("frameRate", getFrameRate());
    writer.putKeyValue("numFrames", getNumFrames());
    
    bool result = doWriteSeq(writer);
    
    writer.endMapping();
    
    return result;
}


bool AbstractSeq::doWriteSeq(YAMLWriter&)
{
    return true;
}


const std::string& AbstractSeq::seqMessage() const
{
    return dummySeqMessage;
}


AbstractMultiSeq::AbstractMultiSeq(const char* seqType)
    : AbstractSeq(seqType)
{

}


AbstractMultiSeq::AbstractMultiSeq(const AbstractMultiSeq& org)
    : AbstractSeq(org)
{

}


AbstractMultiSeq& AbstractMultiSeq::operator=(const AbstractMultiSeq& rhs)
{
    AbstractSeq::operator=(rhs);
    return *this;
}


void AbstractMultiSeq::copySeqProperties(const AbstractMultiSeq& source)
{
    AbstractSeq::copySeqProperties(source);
}


AbstractMultiSeq::~AbstractMultiSeq()
{

}


int AbstractMultiSeq::partIndex(const std::string& /* partLabel*/) const
{
    return -1;
}


const std::string& AbstractMultiSeq::partLabel(int /* partIndex */) const
{
    static const std::string nolabel;
    return nolabel;
}


bool AbstractMultiSeq::doWriteSeq(YAMLWriter& writer)
{
    writer.putKeyValue("numParts", getNumParts());
    return true;
}


std::vector<std::string> AbstractMultiSeq::readSeqPartLabels(const Mapping& archive)
{
    vector<string> labelStrings;
    const Listing& labels = *archive.findListing("partLabels");
    if(labels.isValid()){
        for(int i=0; i < labels.size(); ++i){
            labelStrings.push_back(labels[i].toString());
        }
    }
    return labelStrings;
}


bool AbstractMultiSeq::writeSeqPartLabels(YAMLWriter& writer)
{
    writer.putKey("partLabels");
    writer.startFlowStyleListing();
    int n = getNumParts();
    for(int i=0; i < n; ++i){
        writer.putDoubleQuotedString(partLabel(i));
    }
    writer.endListing();
    return true;
}
