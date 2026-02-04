# Phase 4+ Cleanup - COMPLETE

## Summary
Systematically disabled all remaining MLT/Player/Playlist/Timeline references in mainwindow.cpp that would cause compilation errors due to undefined pointers (m_player, m_playlistDock, m_timelineDock).

## Functions Disabled

### Player/Timeline Navigation & Selection
- **onTimelineClipSelected()** - MLT timeline/player switching (lines 960-967)
  - Disabled: m_player tabIndex check, m_timelineDock saveAndClearSelection, m_player onTabBarClicked

### Producer Management
- **hideProducer()** - MLT producer color replacement logic (lines 2358-2375)
  - Disabled: openCut operations, QScrollArea widget cleanup, m_player reset
- **closeProducer()** - MLT producer closing and cleanup (lines 2376-2383)
  - Disabled: hideProducer call, m_filterController motionTracker, MLT.close, MLT.setSavedProducer
- **onProducerOpened()** - Entire MLT producer opening workflow (lines 3275-3307)
  - Disabled: loadProducerWidget, playlist/multitrack loading, m_player operations

### Status & Messages
- **showStatusMessage(QAction)** - Player status label setting (lines 2386-2392)
  - Disabled: m_statusBarAction reset, m_player setStatusLabel
- **showStatusMessage(const QString)** - Player status label with color role (lines 2394-2403)
  - Disabled: action creation and m_player setStatusLabel

### Seeking Operations
- **seekPlaylist()** - MLT playlist seeking with player synchronization (lines 2410-2429)
  - Disabled: MLT producer operations, m_player setIn/setOut, jack trigger, onProducerOpened, seek, focus, tab switch
- **seekTimeline()** - MLT timeline seeking with player control (lines 2431-2454)
  - Disabled: multitrack checking, MLT producer operations, m_player operations, focus, tab switch, pause/seek
- **seekKeyframes()** - Player seek for keyframe positioning (lines 2456-2459)
  - Disabled: m_player seek

### Playlist Event Handlers
- **onPlaylistLoaded()** - Marker updates and player tab enabling (lines 3684-3688)
  - Disabled: updateMarkers, m_player enableTab
- **onPlaylistCleared()** - Player tab switching on playlist clear (lines 3690-3694)
  - Disabled: m_player onTabBarClicked (kept setWindowModified as non-MLT)
- **onPlaylistModified()** - Player duration and tab updates on modification (lines 3713-3721)
  - Disabled: m_player onDurationChanged, updateMarkers, enableTab (kept setWindowModified)
- **onPlaylistInChanged()** - Player in point blocking signal updates (lines 5868-5873)
  - Disabled: m_player blockSignals, setIn
- **onPlaylistOutChanged()** - Player out point blocking signal updates (lines 5875-5880)
  - Disabled: m_player blockSignals, setOut

### Multitrack/Timeline Event Handlers
- **onMultitrackCreated()** - Player tab enabling and track transition setup (lines 3723-3728)
  - Disabled: m_player enableTab, m_timelineDock model track transition
- **onMultitrackClosed()** - Full multitrack cleanup with player tab disabling (lines 3730-3744)
  - Disabled: setAudioChannels, setProfile, resetVideoModeMenu, resetSourceUpdated, MLT operations, m_player enableTab
- **onMultitrackModified()** - Timeline dock selection and producer in/out point updates (lines 3746-3806)
  - Disabled: m_timelineDock selection operations, MLT clip info, producer property operations, MLT refreshConsumer (kept setWindowModified)
- **onMultitrackDurationChanged()** - Player duration change notification (lines 3788-3793)
  - Disabled: MLT producer check, m_player onDurationChanged

### Preview & External Monitoring
- **onExternalTriggered()** - Full external monitor/SDI/HDMI output logic (lines 4491-4578)
  - Disabled: External GPU restart logic, MLT stop, m_player moveVideoToScreen, MLT properties, profile changes, progressive option, decklink gamma/keyer menu operations
- **on_actionPreview360_triggered()** - Preview scaling at 360p (lines 5920-5927)
  - Disabled: Settings.setPlayerPreviewScale, setPreviewScale, m_player showIdleStatus
- **on_actionPreview540_triggered()** - Preview scaling at 540p (lines 5929-5936)
  - Disabled: Settings.setPlayerPreviewScale, setPreviewScale, m_player showIdleStatus

### Proxy Management
- **on_actionUseProxy_triggered()** - Full proxy enable/disable workflow (lines 6059-6139)
  - Disabled: Complete proxy on/off workflow including MLT open, XML operations, UI conversions, proxy generation prompts, m_player status

### Misc
- **onClipCopied()** - Player source tab enabling (lines 5095-5098)
  - Disabled: m_player enableTab

## Pattern Applied
All disabled code follows the established convention:
```cpp
// DISABLED: MLT [system]
// Original code commented out
// (void)param; // For unused parameters (to avoid compiler warnings)
```

## Code Quality Notes
- All edits preserve function signatures for forward compatibility
- Unused parameter warnings prevented with (void) statements
- Comments clearly mark disabled sections for future reference
- Non-MLT functionality preserved (e.g., setWindowModified in playlist/multitrack handlers)

## Files Modified
- `src\mainwindow.cpp` - 23 functions systematically disabled

## Compilation Status
✅ Syntax check passed (no diagnostics)
⏳ Full compilation test pending (requires CMake setup)

## Next Steps
1. Complete full cmake/ninja build test
2. Document any remaining linker errors
3. Begin Phase B mail-core implementation:
   - IMAP client module
   - SMTP client module
   - Mail database schema
   - Message model/repository
4. Phase B will preserve all infrastructure:
   - Settings system
   - Database framework
   - JobQueue for async operations
   - Logging system
   - Generic dialogs and UI utilities
