#include "SDK.hpp"

#include <thread>
#include <chrono>

//#include "MinHook.h"
//
//#pragma comment(lib, "libMinHook.x64.lib")

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
        parent->Mesh->SetEnableGravity(!parent->Mesh->IsGravityEnabled());
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

//static void UpdateFlyHack() {
//    static bool enabled = false;
//    if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
//        enabled = !enabled;
//        const auto player = static_cast<SDK::AHTPlayerCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
//        if (enabled) {
//            player->GetHTPlayerMovementComponent()->MaxFlySpeed = 20000.f;
//            player->GetHTPlayerMovementComponent()->MovementMode = SDK::EMovementMode::MOVE_Flying;
//        }
//        else {
//            if (player->GetHTPlayerMovementComponent()->MovementMode == SDK::EMovementMode::MOVE_Flying)
//                player->GetHTPlayerMovementComponent()->MovementMode = SDK::EMovementMode::MOVE_Falling;
//        }
//    }
//
//    if (!enabled) return;
//
//    const auto player = static_cast<SDK::AHTPlayerCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
//    const auto mov = player->GetHTPlayerMovementComponent();
//    if (GetAsyncKeyState(VK_SPACE))
//        mov->AddInputVector({ 0.f, 0.f, 10.f }, true);
//    else if (GetAsyncKeyState(VK_LCONTROL))
//        mov->AddInputVector({ 0.f, 0.f, 10.f }, true);
//}

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
    if (!target) return;

    const auto forward = target->GetActorForwardVector();
    const auto right = target->GetActorRightVector();
    const auto up = target->GetActorUpVector();

    toMove = forward * toMove.X + right * toMove.Y + up * toMove.Z;

    const auto multipier = SDK::UGameplayStatics::GetGlobalTimeDilation(SDK::UWorld::GetWorld());
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        target->K2_TeleportTo(target->K2_GetActorLocation() + toMove * multipier, target->K2_GetActorRotation());
    else
        target->K2_AddActorWorldOffset(toMove * multipier, true, nullptr, false);
}

static void UpdateInvincibleHack() {
    static bool enabled = false;
    if (GetAsyncKeyState(VK_F12) & 1) enabled = !enabled;
    if (!enabled) return;

    const auto world = SDK::UWorld::GetWorld();
    if (!world) return;
    if (!world->OwningGameInstance) return;
    if (!world->OwningGameInstance->LocalPlayers) return;
    if (!world->OwningGameInstance->LocalPlayers[0]) return;
    if (!world->OwningGameInstance->LocalPlayers[0]->PlayerController) return;
    const auto player = static_cast<SDK::AHTAbilityCharacter*>(world->OwningGameInstance->LocalPlayers[0]->PlayerController->Character);
    if (!player) return;

    //if (const auto parent = (SDK::AHTWheeledVehicleDrivable*)player->GetAttachParentActor()) {
    //    parent->bCanBeDamaged = ~parent->bCanBeDamaged;
    //    return;
    //}

    player->bCanBeDamaged = !enabled;
    player->bHaveFallDamage = !enabled;
    //player->HTAbilitySystemComponent->HTAttribute->CooldownReduction.CurrentValue = 0.f;
    //player->HTAbilitySystemComponent->HTAttribute->FinalDamageUp.CurrentValue = 999999.f;
    //player->HTAbilitySystemComponent->ClientIgnoreCooldownsAndCosts(true, true);
    //player->HTAbilitySystemComponent->ClientIgnoreTenacity(true);
}

static void UpdateGhostHack() {
    static bool enabled = false;
    if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
        enabled = !enabled;
        std::cerr << "Set Ghost Mode: " << enabled << std::endl;
    }
    if (!enabled) return;

    const auto player = static_cast<SDK::AHTAbilityCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
    if (!player) return;

    player->CapsuleComponent->SetCollisionEnabled(SDK::ECollisionEnabled::QueryOnly);
    if(const auto mp = player->GetMovementComponent(); !mp->IsMovingOnGround() || mp->IsFalling())
        player->SetActorEnableCollision(false);
    else
        player->SetActorEnableCollision(true);
}

//static SDK::EGetOnVehicleResult (*oo)(void*);

//void (*test_hook_og)(void* Context, void* TheStack, void* Result);
//void test_hook(void* Context, void* TheStack, void* Result){
//    test_hook_og(Context, TheStack, Result);
//    std::cerr << "[CALLED] result: " << *((bool*)Result) << std::endl;
//}
static void UpdatePrinter(){
    if (!(GetAsyncKeyState('P') & 1)) return;

    const auto world = SDK::UWorld::GetWorld();
    const auto player = static_cast<SDK::AHTPlayerCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
    //std::cerr << player->GetName() << std::endl;

    //std::cerr << "========[AttachParent]========" << std::endl;
    //auto parent = player->GetAttachParentActor();
    //while (parent) {
    //    std::cerr << parent->GetName() << std::endl;
    //    parent = parent->GetAttachParentActor();
    //}

    //std::cerr << "========[Components]========" << std::endl;
    //if (const auto parent = player->GetAttachParentActor()) {
    //    
    //}
    //player->K2_DetachFromActor(SDK::EDetachmentRule::KeepWorld, SDK::EDetachmentRule::KeepWorld, SDK::EDetachmentRule::KeepWorld);
    //player->SetActorEnableCollision(true);
    //player->EnableInput(SDK::UGameplayStatics::GetPlayerController(SDK::UWorld::GetWorld(), 0));
    //player->AbilityAttributeSet->Damage.CurrentValue = 99999;
    //player->AbilityAttributeSet->FixedDamageValue.CurrentValue = 99999;
    //player->HTAbilitySystemComponent->MaxHP = 8888;
    //player->HTAbilitySystemComponent->HPCurrent = 8888;
    //player->bCanBeDamaged = false;
    //const auto objs = player->ActiveCustomLogicObjList;
    //for (auto& eff : player->HTAbilitySystemComponent->ActiveGameplayEffects.GameplayEffects_Internal) {
    //    std::cerr << "Active Effect: " << eff.Spec.Def->GetName() << std::endl;
    //    if (eff.Spec.Def->GetName().find("Default__GE_Nanally_WalkOnWall_C") != std::string::npos) {
    //        eff.Spec.Def->DurationPolicy = SDK::EGameplayEffectDurationType::Infinite;
    //        std::cerr << "Nanally Infinite Buff Enabled" << std::endl;
    //    }
    //    eff.Spec.duration += 99999.f;
    //}
    //std::cerr << "N Logic Objs: " << objs.Num() <<std::endl;
    //for(const auto& obj: objs) {
    //    std::cerr << "Obj Name: " << obj->GetName() << std::endl;
    //    if (obj->IsA(SDK::UNanallyLogicObj::StaticClass())) {
    //        const auto buff = ((SDK::UNanallyLogicObj*)obj)->CatBuff.Get();
    //        std::cerr << "Nanally Buff Name: " << buff->GetName() << std::endl;
    //        //((SDK::UHTGamePlayEffect*)buff->ClassDefaultObject)->DurationPolicy = SDK::EGameplayEffectDurationType::Infinite;
    //        //std::cerr << "Nanally Infinite Buff Enabled" << std::endl;
    //    }
    //}
    if (const auto parent = (SDK::AHTWheeledVehicleDrivable*)player->GetAttachParentActor()) {
        //parent->MaxSpeedInKmh *= 10;
        //parent->bActorEnableCollision = false;
        parent->bCanBeDamaged = false;
        parent->Mesh->SetNotifyRigidBodyCollision(false);
        //parent->bEnableVelocityLimit = false;
        //parent->HTVehicleMovementComponent->bEnableTopSpeedLimit = false;
        parent->HTVehicleMovementComponent->TopSpeedInKmH *= 10;
        parent->HTVehicleMovementComponent->Mass /= 5;
        parent->SetFrontWheelFrictionMultiplier(100.f);
        parent->SetRearWheelFrictionMultiplier(100.f);
        // parent->HTVehicleMovementComponent->SetMaxEngineTorque(20 * parent->HTVehicleMovementComponent->GetEngineMaxTorque());
        parent->Mesh->SetAngularDamping(0.1f);
        parent->Mesh->SetLinearDamping(0.1f);
        parent->VehicleMovementComponent->DownforceCoefficient *= 100;


        //UC::TArray<SDK::AActor*> actors{};
        //SDK::UGameplayStatics::GetAllActorsOfClass(world, SDK::AHTAIRaceCheckPoint::StaticClass(), &actors);
        //for (const auto& act : actors) {
        //    const auto p = static_cast<SDK::AHTAIRaceCheckPoint*>(act);
        //    if (p->bIsFinishCheckPoint) continue;
        //    parent->K2_TeleportTo(p->K2_GetActorLocation(), parent->K2_GetActorRotation());
        //}
    }
    //player->SavedPlayerState->RoleLevel = 99;
    //player->SavedPlayerState->FishingLevel = 99;
    //static bool f = false;
    //if (!f) {
    //    f = !f;
    //    player->SavedPlayerState->BP_BeginWashDishes(false);
    //}
    //else {
    //    player->SavedPlayerState->ClientSendWashDishesAwards();
    //    player->SavedPlayerState->ServerSendWashDishesAwards();
    //}
    //for (const auto& appearance : player->SavedPlayerState->UnlockAppearanceIds) {
    //    std::cerr << "Appearance: " << appearance.ToString() << std::endl;
    //}
    //for (auto& quest : player->SavedPlayerState->QuestComponent->QuestsInProgress) {
    //    static std::vector<std::string> cached{};
    //    if (std::find(cached.begin(), cached.end(), quest.QuestId.ToString()) != cached.end()) continue;
    //    cached.push_back(quest.QuestId.ToString());

    //    std::cerr << "Quest: " << quest.QuestId.ToString() << ", Status: " << (int)quest.QuestStatus << std::endl;
    //    if (quest.QuestId.ToString().starts_with("tq")) {
    //        for (auto& prog : quest.ObjectivesProgress) {
    //            //std::cerr << "Objective: " << prog.ObjectiveID.ToString() << ", completed: " << prog.IsCompleted << std::endl;
    //            prog.IsCompleted = true;
    //        }
    //        quest.QuestStatus = SDK::EQuestStatus::EQS_CAN_SUBMIT;
    //        player->SavedPlayerState->QuestComponent->SubmitQuest(quest.QuestId);
    //    }
    //}
    ////const auto method = player->SavedPlayerState->Class->GetFunction("HTPlayerState", "IsFleetUnlocked");
    
    //const auto controller = player->GetHTPlayerMovementComponent();
    //const auto controller = player->SelfHTPlayerController;
    //SDK::FHitResult hit;
    //if (controller->GetHitResultUnderCursorByChannel(SDK::ETraceTypeQuery::TraceTypeQuery1, true, &hit)) {
    //    if (const auto c = hit.Component.Get()) {
    //        const auto actor = c->GetOwner();
    //        std::cerr << "Actor: Name: " << actor->GetName() << " Class: " << actor->Class->GetName() << std::endl;
    //        for (const auto& c : actor->InstanceComponents) {
    //            std::cerr << "Component: Name: " << c->GetName() << " Class: " << c->Class->GetName() << std::endl;
    //        }
    //    }
    //}

    //SDK::UHTVehicleSummonSubsystem::GetHTVehicleSummonSubsystem(world);

    //controller->SendToConsole(SDK::FString(L"EnableCheats"));
    //controller->EnableCheats();
    //const auto CM = static_cast<SDK::UHTCheatManager*>(controller->CheatManager);
    //if (!CM) {
    //    std::cerr << "No Cheat Manager Found\n";
    //    return;
    //}
    ////SDK::FTextImpl::FTextData td{.TextSource = SDK::FString("")};
    ////CM->AddSystemMessage(SDK::FText{.TextData = &td});
    //CM->UnlockAllAppearance();
}

static void UpdateTestHack() {
    if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
        const auto world = SDK::UWorld::GetWorld();
        const auto player = static_cast<SDK::AHTPlayerCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
        UC::TArray<SDK::AActor*> actors{};
        SDK::UGameplayStatics::GetAllActorsOfClass(world, SDK::AHTMonsterCharacter::StaticClass(), &actors);
        std::cerr << "N Monster: " << actors.Num() << std::endl;
        for (const auto& act : actors) {
            const auto m = static_cast<SDK::AHTMonsterCharacter*>(act);
            //if (!m->bCanBeDamaged) continue;
            //SDK::UGameplayStatics::ApplyDamage(m, 999999.f, player->Controller, player, SDK::UHTDamageType::StaticClass());
            m->SetIsDead(true);
            //SDK::UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(m, SDK::FGameplayTag{}, SDK::FGameplayEventData{});
        }
    }
    else if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
        const auto world = SDK::UWorld::GetWorld();
        const auto player = static_cast<SDK::AHTPlayerCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0));
        //UC::TArray<SDK::AActor*> actors{};
        //SDK::UGameplayStatics::GetAllActorsOfClass(world, SDK::AHTHarvestActor::StaticClass(), &actors);
        //std::cerr << "N Harvest: " << actors.Num() << std::endl;
        //for (const auto& act : actors) {
        //    const auto i = static_cast<SDK::AHTHarvestActor*>(act);
        //    i->HTItemNetInfo.Amount *= 50;
        //    player->K2_TeleportTo(i->K2_GetActorLocation(), player->K2_GetActorRotation());
        //    break;
        //}
        UC::TArray<SDK::AActor*> actors{};
        SDK::UGameplayStatics::GetAllActorsOfClass(world, SDK::AHTQuestTriggerBox::StaticClass(), &actors);
        std::cerr << "N Quest Trigger: " << actors.Num() << std::endl;
        for (const auto& act : actors) {
            const auto q = static_cast<SDK::AHTQuestTriggerBox*>(act);
            player->K2_TeleportTo(q->K2_GetActorLocation(), player->K2_GetActorRotation());
            break;
        }
    }
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

//void (*ProcessEventOG)(SDK::UObject* Object, SDK::UFunction* Function, void* Params);
//static void ProcessEvent(SDK::UObject* Object, SDK::UFunction* Function, void* Params)
//{
//    using namespace std::chrono;
//    static auto lastTime = steady_clock::now();
//    const auto now = steady_clock::now();
//    if (duration_cast<milliseconds>(now - lastTime).count() >= 16) {
//        if (const auto player = static_cast<SDK::AHTAbilityCharacter*>(SDK::UGameplayStatics::GetPlayerCharacter(SDK::UWorld::GetWorld(), 0))) {
//            player->HTAbilitySystemComponent->ClientIgnoreCooldownsAndCosts(true, true);
//            player->HTAbilitySystemComponent->ClientIgnoreTenacity(true);
//        }
//
//        lastTime = now;
//    }
//
//    ProcessEventOG(Object, Function, Params);
//}

//#include <fstream>
//void DumpModuleByAddress(void* addr)
//{
//    MEMORY_BASIC_INFORMATION mbi;
//
//    if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
//        return;
//
//    char path[MAX_PATH] = "";
//
//    if (!GetModuleFileNameA(
//        (HMODULE)mbi.AllocationBase,
//        path,
//        MAX_PATH))
//    {
//        // path = "";
//    }
//
//    std::cerr << "Module: " << path << "Size: " << mbi.RegionSize << std::endl;
//
//    if (mbi.State != MEM_COMMIT) {
//        std::cerr << "Memory not committed\n";
//        return;
//    }
//
//    if (mbi.Protect & PAGE_NOACCESS || mbi.Protect & PAGE_GUARD) {
//        std::cerr << "Memory not readable\n";
//        return;
//    }
//
//    char filename[64];
//    sprintf_s(filename, "dump_%p.out", mbi.AllocationBase);
//    std::ofstream file(filename, std::ios::binary);
//    file.write(reinterpret_cast<const char*>(mbi.BaseAddress), mbi.RegionSize);
//    file.close();
//
//    std::cerr << "Dump Success\n";
//}

static void Bootstrap(HMODULE Module) {
    while (!(GetAsyncKeyState(VK_INSERT) & 1))
        std::this_thread::sleep_for(100ms);

    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stderr);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    std::cerr << "Initializing [NTE_Hack]\n";

    //std::cerr << "Press Insert to Start.\n";
    //while(!(GetAsyncKeyState(VK_INSERT) & 1))
    //    std::this_thread::sleep_for(100ms);

    std::cerr << "Start Hacking...\n";
    std::this_thread::sleep_for(1s);

    //DumpModuleByAddress(SDK::UWorld::GetWorld());

    //MH_Initialize();
    //MH_CreateHook((LPVOID)(SDK::InSDKUtils::GetImageBase() + SDK::Offsets::ProcessEvent), &ProcessEvent, (LPVOID*)&ProcessEventOG);
    //MH_EnableHook((LPVOID)(SDK::InSDKUtils::GetImageBase() + SDK::Offsets::ProcessEvent));

    for (;;) {
        std::this_thread::sleep_for(16.7ms);

        //static bool enabled = true;
        //if (GetAsyncKeyState(VK_INSERT) & 1) {
        //    enabled = !enabled;
        //    if (enabled) std::cerr << "Enabled [NTE_Hack]\n";
        //    else std::cerr << "Disabled [NTE_Hack]\n";
        //}
        //if (!enabled) continue;

        UpdateSpeedHack();
        UpdateScaleHack();
        UpdateFlyHack();
        UpdateMovementHack();
        UpdateTeleportHack();
        UpdateInvincibleHack();
        UpdatePrinter();
        //UpdateConsoleHack();
        UpdateGhostHack();
        UpdateTestHack();
    }

    fclose(stderr);
    if (Dummy) fclose(Dummy);
    FreeConsole();

    FreeLibraryAndExitThread(Module, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        std::thread(Bootstrap, hModule).detach();
        break;
    }
    return TRUE;
}