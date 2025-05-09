#pragma once
#include "SandboxAPI.h"
#include "Engine/GameApplication.h"

class SANDBOX_API SandboxGameApplication : public QE::GameApplication
{
public:
    SandboxGameApplication() = default;
    virtual ~SandboxGameApplication() = default;

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void Update() override;
};