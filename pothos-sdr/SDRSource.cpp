// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "SDRBlock.hpp"

class SDRSource : public SDRBlock
{
public:
    static Block *make(const Pothos::DType &dtype, const std::vector<size_t> &channels)
    {
        if (SoapySDR::getABIVersion() != SOAPY_SDR_ABI_VERSION) throw Pothos::Exception("SDRSource::make()",
            Poco::format("Failed ABI check. Pothos SDR %s. Soapy SDR %s. Rebuild the module.",
            std::string(SOAPY_SDR_ABI_VERSION), SoapySDR::getABIVersion()));
        return new SDRSource(dtype, channels);
    }

    SDRSource(const Pothos::DType &dtype, const std::vector<size_t> &channels):
        SDRBlock(SOAPY_SDR_RX, channels)
    {
        for (size_t i = 0; i < channels.size(); i++) this->setupOutput(i, dtype);
    }

    /*******************************************************************
     * Streaming implementation
     ******************************************************************/
    void activate(void)
    {
        if (not this->isReady())
        {
            throw Pothos::Exception("SDRSource::activate()", "device not ready");
        }
        //TODO other arguments
        //TODO check result
        _device->activateStream(_stream);

        //emit configuration TODO
        //this->callVoid("getSampleRateTriggered", this->getSampleRate());
        //this->callVoid("getSampleRatesTriggered", this->getSampleRates());
        //this->callVoid("getFrontendMapTriggered", this->getFrontendMap());
        for (size_t i = 0; i < _channels.size(); i++)
        {
            const auto chanStr = std::to_string(i);
        }
    }

    void deactivate(void)
    {
        //TODO check result
        _device->deactivateStream(_stream);
    }

    void work(void)
    {
        int flags = 0;
        long long timeNs = 0;
        const size_t numElems = this->workInfo().minOutElements;
        const long timeoutUs = this->workInfo().maxTimeoutNs/1000;
        const auto &buffs = this->workInfo().outputPointers;
        const int ret = _device->readStream(_stream, buffs.data(), numElems, flags, timeNs, timeoutUs);

        //TODO labels

        if (ret > 0) for (auto output : this->outputs()) output->produce(numElems);
        else if (ret == SOAPY_SDR_TIMEOUT) return this->yield();
        else poco_error_f1(Poco::Logger::get("SDRSource"), "readStream %d", ret);
    }
};

static Pothos::BlockRegistry registerSDRSource(
    "/sdr/source", &SDRSource::make);
