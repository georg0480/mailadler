# Phase B - Mail-Adler Core Architecture Planning

## Current Status (Post-Phase 4+ Cleanup)
- ✅ All video-specific dependencies removed from CMakeLists.txt  
- ✅ All MLT/Video code disabled in main.cpp, mainwindow.h, mainwindow.cpp
- ✅ All Player/Playlist/Timeline references disabled (23 functions)
- ✅ Syntax validation passed
- ⏳ Full compilation testing pending (CMake/Ninja setup)

## Preserved Infrastructure (Phase A+4 Success)
These components remain functional and will support mail-client operations:

### Database Layer
- **File:** `src/database.cpp`, `src/database.h`
- **Purpose:** SQLite3 database persistence
- **Use Case:** Mail storage, account configurations, folder hierarchy
- **Status:** Ready to extend with mail schema

### Job Queue
- **File:** `src/jobqueue.cpp`, `src/jobqueue.h`
- **Purpose:** Asynchronous task queue
- **Use Case:** Background mail sync, folder updates, message download
- **Status:** Ready for IMAP/SMTP operations

### Settings System
- **File:** `src/settings.cpp`, `src/settings.h`
- **Purpose:** Application preferences persistence
- **Use Case:** Account credentials, UI preferences, sync intervals
- **Status:** Extensible for mail-specific settings

### Logging
- **File:** `CuteLogger/` module
- **Purpose:** Debug and operation logging
- **Status:** Available for mail operations

### UI Framework
- **Base:** Qt6 Widgets (already in use)
- **Status:** Main window, dialogs, dock widgets functional

## Phase B Deliverables

### 1. Mail Data Model & Schema
**Objective:** Define mail storage structure

**Components:**
- `src/models/MailMessage.h/cpp` - Email message entity
  - From, To, Subject, Body, Date, Flags (Read, Starred, Spam)
  - UID, Folder ID, Account ID
  - Attachments metadata
  
- `src/models/MailFolder.h/cpp` - IMAP folder entity
  - Folder name, path, flags
  - Read/unread counts
  - Sync state tracking
  
- `src/models/MailAccount.h/cpp` - Email account entity
  - IMAP server settings (host, port, auth)
  - SMTP server settings
  - Sync preferences (interval, folders)

- `src/database/MailSchema.h/cpp` - SQLite schema
  - Tables: accounts, folders, messages, attachments, sync_state
  - Indexes for fast queries
  - Migration system

### 2. IMAP Client Module
**Objective:** Email retrieval and folder management

**Components:**
- `src/imap/ImapClient.h/cpp` - IMAP protocol wrapper
  - Connection management
  - Authentication (LOGIN, PLAIN, OAuth2 skeleton)
  - Folder enumeration
  - Message fetch (headers + body)
  - UID tracking for sync
  
- `src/imap/ImapFolder.h/cpp` - Folder operations
  - Folder status (EXISTS, RECENT, UNSEEN)
  - Select/close operations
  - Message search (by date, sender, etc.)
  
- `src/imap/ImapSync.h/cpp` - Incremental sync engine
  - Track last sync state
  - Fetch new messages only
  - Handle flag changes (read/star status)
  - Conflict resolution

### 3. SMTP Client Module
**Objective:** Email sending capability

**Components:**
- `src/smtp/SmtpClient.h/cpp` - SMTP protocol wrapper
  - Connection management
  - Authentication
  - Message sending
  - Error handling
  
- `src/smtp/MessageComposer.h/cpp` - Compose operations
  - Build MIME messages
  - Handle attachments
  - Quote/reply operations

### 4. Account Manager
**Objective:** Multi-account support

**Components:**
- `src/account/AccountManager.h/cpp`
  - Add/remove/edit accounts
  - Store credentials securely (encrypted)
  - Account switching
  
- `src/account/CredentialStorage.h/cpp`
  - Platform-specific secure storage (Windows: DPAPI, Linux: keyring)
  - Credential caching
  - Token refresh (OAuth2)

### 5. Mail Synchronization Service
**Objective:** Background sync automation

**Components:**
- `src/sync/SyncService.h/cpp`
  - Periodic sync scheduling
  - JobQueue integration
  - Error recovery
  - Conflict resolution
  
- `src/sync/SyncScheduler.h/cpp`
  - Interval-based sync
  - Manual sync triggers
  - Push notification support (IMAP IDLE)

### 6. UI Layer (Phase B Minimal)
**Objective:** Basic mail client interface

**Components:**
- `src/ui/MailListWidget.h/cpp` - Message list view
  - TableView with sender, subject, date, flags
  - Threading support skeleton
  
- `src/ui/MailViewWidget.h/cpp` - Message viewer
  - Display full message content
  - Handle HTML/plain text
  - Attachment preview/download
  
- `src/ui/ComposeDialog.h/cpp` - Message composition
  - Text editor
  - Recipient fields
  - Attachment management
  
- `src/ui/AccountSetupDialog.h/cpp` - Account configuration
  - Server details
  - Authentication
  - Folder selection

## Phase B Architecture Diagram

```
┌─────────────────────────────────────────────────────────┐
│              Mail-Adler Main Window                      │
├─────────────────────────────────────────────────────────┤
│  Preserved: StatusBar, Menus, Toolbars, Settings        │
│  New: Mail Views (List, Compose, Account Management)   │
└─────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────┐
│                  UI Components (Phase B)                 │
├─────────────────────────────────────────────────────────┤
│  - MailListWidget                                        │
│  - MailViewWidget                                        │
│  - ComposeDialog                                         │
│  - AccountSetupDialog                                    │
└─────────────────────────────────────────────────────────┘
         ↓ (signals/slots)
┌─────────────────────────────────────────────────────────┐
│             Account Manager & Sync Service              │
├─────────────────────────────────────────────────────────┤
│  - AccountManager (add/remove/switch)                   │
│  - SyncService (background sync)                        │
│  - CredentialStorage (secure auth)                      │
└─────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────┐
│          IMAP & SMTP Client Modules                     │
├─────────────────────────────────────────────────────────┤
│  - ImapClient (fetch, sync, folder ops)                 │
│  - SmtpClient (send)                                    │
│  - ImapSync (incremental sync engine)                   │
└─────────────────────────────────────────────────────────┘
         ↓ (database ops)
┌─────────────────────────────────────────────────────────┐
│            Mail Data Models & Database                  │
├─────────────────────────────────────────────────────────┤
│  - MailMessage, MailFolder, MailAccount                 │
│  - MailSchema (SQLite tables)                           │
│  - Database (from Phase A - persistence layer)          │
└─────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────┐
│              Infrastructure Layer                        │
├─────────────────────────────────────────────────────────┤
│  - JobQueue (async mail operations)                     │
│  - Settings (app config)                                │
│  - Logging (debug/audit)                                │
└─────────────────────────────────────────────────────────┘
```

## Phase B Implementation Order

### Milestone 1: Data Layer
1. **MailMessage, MailFolder, MailAccount** models
2. **MailSchema** - Create SQLite tables and migrations
3. **Database schema validation**

### Milestone 2: IMAP Module
1. **ImapClient** - Basic connection, authentication, folder list
2. **ImapFolder** - Folder operations (select, status)
3. **ImapSync** - UID tracking, incremental fetch
4. **Testing** - Mock IMAP servers (e.g., test.example.com)

### Milestone 3: SMTP Module
1. **SmtpClient** - Connection, auth, basic send
2. **MessageComposer** - MIME message building
3. **Error handling & retries**

### Milestone 4: Account & Sync Services
1. **AccountManager** - Add/remove/switch accounts
2. **CredentialStorage** - Secure credential handling
3. **SyncService** - JobQueue integration, periodic sync
4. **SyncScheduler** - Background sync automation

### Milestone 5: Basic UI
1. **MailListWidget** - Display messages
2. **MailViewWidget** - Read messages
3. **ComposeDialog** - Send messages
4. **AccountSetupDialog** - Configure accounts

## CMakeLists.txt Updates Needed

### Add New Directories
```cmake
add_subdirectory(src/models)
add_subdirectory(src/database)
add_subdirectory(src/imap)
add_subdirectory(src/smtp)
add_subdirectory(src/account)
add_subdirectory(src/sync)
add_subdirectory(src/ui)
```

### External Dependencies (Already Available)
- Qt6::Core
- Qt6::Sql
- Qt6::Network (for IMAP/SMTP connections)
- Qt6::Widgets

### Potential New Dependencies
- **libsasl2** (SMTP authentication)
- **openssl** (TLS/SSL for secure connections)
- **libkeyring** (Linux credential storage)

## Success Criteria

✅ **Phase B Complete When:**
- All 6 components (Data, IMAP, SMTP, Account, Sync, UI) implemented
- Single account IMAP sync functional
- Send email via SMTP functional
- Messages persist in SQLite database
- Background sync runs without blocking UI
- Settings saved across sessions

## Next Steps After Phase B
- Phase C: Multi-account support enhancement
- Phase D: IMAP IDLE push notifications
- Phase E: Advanced search, filters, threading
- Phase F: Attachments download/display
- Phase G: Encryption (PGP/S-MIME) support
