#include "SDK.hpp"

#include <thread>
#include <chrono>

//#include "MinHook.h"
//
//#pragma comment(lib, "libMinHook.x64.lib")

//void (*ProcessEventOG)(SDK::UObject* Object, SDK::UFunction* Function, void* Params);
//void ProcessEvent(SDK::UObject* Object, SDK::UFunction* Function, void* Params)
//{
//    ProcessEventOG(Object, Function, Params);
//    const auto FuncName = Function->GetName();
//
//    if (FuncName.find("CanGetOffVehicle") != FuncName.npos)
//    {
//        std::cerr << "Object Name: " << Object->GetFullName() << ", Function Name: " << Function->GetFullName() << std::endl;
//        std::cerr << "Function Short Name: " << Function->GetName() << std::endl;
//    }
//}
//
//static void UpdateVehicleHack() {
//    static bool hooked = false;
//
//    if (hooked) return;
//
//    __int64 BaseAddr = __int64(GetModuleHandleW(0));
//    MH_CreateHook((LPVOID)(BaseAddr + SDK::Offsets::ProcessEvent), ProcessEvent, (LPVOID*)&ProcessEventOG);
//    MH_EnableHook(0);
//
//    hooked = true;
//}

using namespace std::chrono_literals;

static void UpdateSpeedHack() {
    if (GetAsyncKeyState(VK_OEM_PLUS) & 1) {
        const auto world = SDK::UWorld::GetWorld();
        const auto curSpeed = SDK::UGameplayStatics::GetGlobalTimeDilation(world);
        SDK::UGameplayStatics::SetGlobalTimeDilation(world, curSpeed + 1);
        std::cerr << "Set Global Time Dilation: " << curSpeed << std::endl;;
    }
    else if (GetAsyncKeyState(VK_OEM_MINUS) & 1) {
        const auto world = SDK::UWorld::GetWorld();
        const auto curSpeed = SDK::UGameplayStatics::GetGlobalTimeDilation(world);
        SDK::UGameplayStatics::SetGlobalTimeDilation(world, curSpeed - 1);
        std::cerr << "Set Global Time Dilation: " << curSpeed << std::endl;;
    }
    else if (GetAsyncKeyState(VK_BACK) & 1) {
        SDK::UGameplayStatics::SetGlobalTimeDilation(SDK::UWorld::GetWorld(), 1);
        std::cerr << "Reset Global Time Dilation: " << 1 << std::endl;;
    }
}

static void UpdateScaleHack() {
    //float multipier{};

    //if (GetAsyncKeyState(VK_ADD) & 1) multipier = 2;
    //else if (GetAsyncKeyState(VK_SUBTRACT) & 1) multipier = 0.5;
    //else return;

    //const auto player = SDK::UWorld::GetWorld()->OwningGameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn;
    //player->SetActorScale3D(player->GetActorScale3D() * multipier);
    //std::cerr << "Set Player Scale: " << "*= " << multipier << std::endl;;

    if (GetAsyncKeyState(VK_ADD) & 1) {
        const auto player = SDK::UWorld::GetWorld()->OwningGameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn;
        player->SetActorScale3D(player->GetActorScale3D() * 2);
        std::cerr << "Set Player Scale: " << "*= 2" << std::endl;;
        //if (const auto parent = (SDK::AHTWheeledVehicleDrivable*)player->GetAttachParentActor()) {
        //    parent->SetActorScale3D(parent->GetActorScale3D() * 2);
        //    std::cerr << "Set Parent Scale: " << "*= 2" << std::endl;;
        //}
    }
    else if (GetAsyncKeyState(VK_SUBTRACT) & 1) {
        const auto player = SDK::UWorld::GetWorld()->OwningGameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn;
        player->SetActorScale3D(player->GetActorScale3D() * 0.5);
        std::cerr << "Set Player Scale: " << "*= 0.5" << std::endl;
        //if (const auto parent = (SDK::AHTWheeledVehicleDrivable*)player->GetAttachParentActor()) {
        //    parent->SetActorScale3D(parent->GetActorScale3D() * 0.5);
        //    std::cerr << "Set Parent Scale: " << "*= 0.5" << std::endl;;
        //}
    }
}

static void UpdateFlyHack() {
    if (!(GetAsyncKeyState(VK_NUMPAD0) & 1)) return;

    static bool flying = false;
    static float cachedGravity{};

    const auto player = SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0);

    if (const auto parent = (SDK::AHTWheeledVehicleDrivable*)player->GetAttachParentActor()) {
        parent->bActorEnableCollision = ~parent->bActorEnableCollision;
        return;
    }

    if (!flying) {
        cachedGravity = player->CharacterMovement->GravityScale;
        player->CharacterMovement->GravityScale = 0.f;
        std::cerr << "Enable Flying" << std::endl;
    }
    else {
        player->CharacterMovement->GravityScale = cachedGravity;
        std::cerr << "Disable Flying" << std::endl;
    }
    flying = !flying;
}

static void UpdateTeleportHack() {
    static SDK::FVector loc(0, 0, 0);
    static SDK::FRotator rot(0, 0, 0);

    if (GetAsyncKeyState(VK_F6) & 1) {
        if (!loc.IsZero()) {
            auto target = static_cast<SDK::AActor*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
            if (const auto parent = target->GetAttachParentActor()) target = parent;
            target->K2_TeleportTo(loc, rot);
            std::cerr << "Teleport to " << loc.X << " " << loc.Y << " " << loc.Z << std::endl;
        }
    }
    else if (GetAsyncKeyState(VK_F7) & 1) {
        auto target = static_cast<SDK::AActor*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
        if (const auto parent = target->GetAttachParentActor()) target = parent;
        loc = target->K2_GetActorLocation();
        rot = target->K2_GetActorRotation();
        std::cerr << "Set Teleport Point " << loc.X << " " << loc.Y << " " << loc.Z << std::endl;
    }
}

static void UpdateMovementHack(){
    SDK::FVector toMove(0, 0, 0);

    if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) toMove.Z += 200;
    if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) toMove.Z -= 200;
    if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000) toMove.X += 200;
    if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) toMove.X -= 200;
    if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000) toMove.Y += 200;
    if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000) toMove.Y -= 200;

    if (toMove.IsZero()) return;
    auto target = static_cast<SDK::AActor*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
    if (const auto parent = target->GetAttachParentActor()) target = parent;

    const auto forward = target->GetActorForwardVector();
    const auto right = target->GetActorRightVector();
    const auto up = target->GetActorUpVector();

    toMove = forward * toMove.X + right * toMove.Y + up * toMove.Z;

    const auto multipier = SDK::UGameplayStatics::GetGlobalTimeDilation(SDK::UWorld::GetWorld());
    target->K2_AddActorWorldOffset(toMove * multipier, true, nullptr, false);
}

static void UpdateInvincibleHack() {
    if (!(GetAsyncKeyState(VK_F12) & 1)) return;

    const auto player = static_cast<SDK::AHTAbilityCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));

    if (const auto parent = (SDK::AHTWheeledVehicleDrivable*)player->GetAttachParentActor()) {
        parent->bCanBeDamaged = ~parent->bCanBeDamaged;
        return;
    }

    player->bCanBeDamaged = ~player->bCanBeDamaged;
    player->bHaveFallDamage = !player->bHaveFallDamage;
}

static void UpdatePrinter(){
    if (!(GetAsyncKeyState('P') & 1)) return;

    const auto world = SDK::UWorld::GetWorld();
    const auto player = static_cast<SDK::AHTAbilityCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
    std::cerr << player->GetName() << std::endl;

    std::cerr << "========[AttachParent]========" << std::endl;
    auto parent = player->GetAttachParentActor();
    while (parent) {
        std::cerr << parent->GetName() << std::endl;
        parent = parent->GetAttachParentActor();
    }

    std::cerr << "========[Components]========" << std::endl;
    if (const auto parent = player->GetAttachParentActor()) {
        
    }
    //player->K2_DetachFromActor(SDK::EDetachmentRule::KeepWorld, SDK::EDetachmentRule::KeepWorld, SDK::EDetachmentRule::KeepWorld);
    //player->SetActorEnableCollision(true);
    //player->EnableInput(SDK::UGameplayStatics::GetPlayerController(SDK::UWorld::GetWorld(), 0));
    player->AbilityAttributeSet->Damage.CurrentValue = 99999;
    player->AbilityAttributeSet->FixedDamageValue.CurrentValue = 99999;
    //player->HTAbilitySystemComponent->MaxHP = 8888;
    //player->HTAbilitySystemComponent->HPCurrent = 8888;
    player->bCanBeDamaged = false;
}

static void UpdateConsoleHack() {
    if (!(GetAsyncKeyState(VK_F11) & 1)) return;

    const auto engine = SDK::UEngine::GetEngine();
    if (engine->ConsoleClass && engine->GameViewport) {
        if (const auto NewObject = static_cast<SDK::UConsole*>(SDK::UGameplayStatics::SpawnObject(engine->ConsoleClass, engine->GameViewport))) {
            engine->GameViewport->ViewportConsole = NewObject;
            std::cerr << "Construct Console: Console object constructed.\n";
        }
        else {
            std::cerr << "Construct Console: Failed to construct console object.\n";
            return;
        }
    }
    else {
        std::cerr << "Construct Console: Failed to construct console object - ConsoleClass or GameViewport is null.\n";
        return;
    }

    // Log console key
    if (SDK::UInputSettings::GetInputSettings()->ConsoleKeys && SDK::UInputSettings::GetInputSettings()->ConsoleKeys.Num() > 0) {
        std::cerr << "Construct Console: Console enabled - access it using key: " << SDK::UInputSettings::GetInputSettings()->ConsoleKeys[0].KeyName.ToString() << ".\n";
    }
    else {
        std::cerr << "Console enabled but no console key is bound.\nAdd this to %LOCALAPPDATA%\\b1\\Saved\\Config\\Windows\\Input.ini -\n[/Script/Engine.InputSettings]\nConsoleKeys = Tilde\n";
    }
}

static void Bootstrap() {
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stderr);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    std::cerr << "Initializing [NTE_Hack]\n";

    std::cerr << "Press Insert to Start.\n";
    while(!(GetAsyncKeyState(VK_INSERT) & 1))
        std::this_thread::sleep_for(100ms);

    std::cerr << "Start Hacking...\n";
    std::this_thread::sleep_for(1s);

    for (;;) {
        std::this_thread::sleep_for(16.7ms);

        UpdateSpeedHack();
        UpdateScaleHack();
        UpdateFlyHack();
        UpdateMovementHack();
        UpdateTeleportHack();
        UpdateInvincibleHack();
        UpdatePrinter();
        UpdateConsoleHack();
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        std::thread(Bootstrap).detach();
        break;
    }
    return TRUE;
}