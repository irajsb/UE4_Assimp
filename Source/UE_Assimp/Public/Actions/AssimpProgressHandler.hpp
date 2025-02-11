#pragma once

#include <assimp/ProgressHandler.hpp>
#include <atomic>

class FAssimpProgressHandler : public Assimp::ProgressHandler {
public:
    FAssimpProgressHandler() : bTerminationRequested(false) {}

    virtual bool Update(float Percentage) override {
        if (bTerminationRequested) {
            return false; // Return false to cancel the import process
        }
        // Perform any other progress handling tasks here
        return true; // Continue the import process
    }

    void RequestTermination() {
        bTerminationRequested.store(true);
    }

    bool IsTerminationRequested() const {
        return bTerminationRequested.load();
    }

private:
    std::atomic<bool> bTerminationRequested;
};