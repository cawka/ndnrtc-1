//
//  ndnrtcTOP_base.hpp
//  ndnrtcTOP
//
//  Created by Peter Gusev on 2/16/18.
//  Copyright © 2018 UCLA REMAP. All rights reserved.
//

#ifndef ndnrtcTOP_base_hpp
#define ndnrtcTOP_base_hpp

#include "TOP_CPlusPlusBase.h"

#include <stdlib.h>
#include <string>
#include <queue>
#include <set>
#include <functional>
#include <boost/shared_ptr.hpp>

#include <ndnrtc/simple-log.hpp>

namespace ndnrtc {
    class IStream;
    
    namespace statistics {
        class StatisticsStorage;
    }
    
    namespace helpers {
        class KeyChainManager;
        class FaceProcessor;
    }
}

namespace ndnlog {
    namespace new_api {
        class Logger;
    }
}

class ndnrtcTOPbase : public TOP_CPlusPlusBase,
                      public ndnlog::new_api::ILoggingObject
{
public:
    static const std::string SigningIdentityName;
    static const std::string NdnRtcStreamName;
    static const std::string NdnRtcTrheadName;
    
    typedef struct _Params {
        bool useMacOsKeyChain_;
        std::string nfdHost_, signingIdentity_;
    } Params;
    
    ndnrtcTOPbase(const OP_NodeInfo *info);
    virtual ~ndnrtcTOPbase();
    
    virtual void        execute(const TOP_OutputFormatSpecs*,
                                OP_Inputs*,
                                TOP_Context* context) override;
    
    virtual int32_t     getNumInfoCHOPChans() override { return 0; }
    virtual void        getInfoCHOPChan(int32_t index,
                                        OP_InfoCHOPChan *chan) override {}
    
    virtual bool        getInfoDATSize(OP_InfoDATSize *infoSize) override;
    virtual void        getInfoDATEntries(int32_t index,
                                          int32_t nEntries,
                                          OP_InfoDATEntries *entries) override;
    
    virtual void        setupParameters(OP_ParameterManager *manager) override;
    virtual void        pulsePressed(const char *name) override;
    
    const char*         getWarningString() override;
    const char*         getErrorString() override;
    
    const OP_NodeInfo*  getNodeInfo() const { return nodeInfo_; }
    
    // returns interaction of two sets
    static std::set<std::string> intersect(const std::set<std::string>&, const std::set<std::string>&);
    
protected:
    const OP_NodeInfo        *nodeInfo_;
    std::string              opName_;
    std::set<std::string>    reinitParams_; // defines parameters that are crucial for reinit
    Params                   *params_;
    
    // FIFO Queue of callbbacks that will be called from within execute() method.
    // Queue will be executed until empty.
    // Callbacks should follow certain signature
    typedef std::function<void(const TOP_OutputFormatSpecs*,
                               OP_Inputs*,
                               TOP_Context *)> ExecuteCallback;
    std::queue<ExecuteCallback> executeQueue_;
    
    boost::shared_ptr<ndnrtc::helpers::FaceProcessor>     faceProcessor_;
    boost::shared_ptr<ndnrtc::helpers::KeyChainManager>   keyChainManager_;
    
    std::string             errorString_, warningString_;
    
    boost::shared_ptr<ndnrtc::IStream>      stream_;
    ndnrtc::statistics::StatisticsStorage*  statStorage_;
    
    virtual void            init();
    virtual void            initStream() {}
    void                    initKeyChainManager(const TOP_OutputFormatSpecs*,
                                                OP_Inputs*,
                                                TOP_Context *);
    void                    initFace(const TOP_OutputFormatSpecs*,
                                     OP_Inputs*,
                                     TOP_Context *);
    void                    registerPrefix(const TOP_OutputFormatSpecs*,
                                           OP_Inputs*,
                                           TOP_Context *);
    
    // returns inputs names that have new values
    virtual std::set<std::string> checkInputs(const TOP_OutputFormatSpecs*, OP_Inputs*, TOP_Context *);

    std::string             extractOpName(std::string oPpath) const;
    void                    readStreamStats();
    std::string             readBasePrefix(OP_Inputs* inputs) const;
    void                    logSink(const char *msg);
    
    /**
     * Flip and convert RGBA frame
     */
    void                    flipFrame(int width, int height, uint8_t* buffer,
                                      bool flipH, bool flipV, bool convertToArgb);
};

#endif /* ndnrtcTOP_base_hpp */