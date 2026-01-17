# Visual Features & Design

## 🎨 Dark Theme Design

The DLL Injector GUI features a modern **Windows 11 inspired dark theme** with careful attention to detail:

### Color Palette

- **Background**: Deep dark gray (`RGB(32, 32, 32)`)
- **Controls**: Slightly lighter (`RGB(40, 40, 40)`)
- **Text**: Pure white (`RGB(255, 255, 255)`)
- **Accent**: Windows blue (`RGB(0, 120, 212)`)
- **Borders**: Subtle gray (`RGB(60, 60, 60)`)

### Typography

- **Font Family**: Segoe UI (Windows 11 system font)
- **Font Sizes**: 
  - Headers: 16pt Semibold
  - Body: 16pt Regular
- **Rendering**: ClearType quality for crisp text

### Visual Elements

#### 🔘 Buttons
- **Rounded corners** (6px radius)
- **Smooth hover effects** - Background color transitions
- **Accent-colored action buttons** - Inject/Unload stand out with blue
- **Owner-draw implementation** - Custom rendering for perfect dark theme
- **Visual states**:
  - Normal: Dark gray background
  - Hover: Lighter gray
  - Pressed: Even darker
  - Action buttons: Blue with hover variation

#### 📊 List View
- **Dark background** matching window theme
- **Grid lines** for better readability
- **Full row selection** 
- **Double buffering** for smooth rendering
- **Color-coded columns**:
  - PID: Monospace feel
  - Process Name: Primary info
  - Architecture: x64/x86 badges
  - Status: ✓ or ✗ with explanation

#### 📝 Input Fields
- **Dark backgrounds** with subtle borders
- **High contrast text** for readability
- **Rounded corners** matching button style
- **Client edge** for depth

#### 🎯 Icons & Emojis
- 🔧 Tool icon for title
- 💉 Syringe for injection field
- 📤 Upload for unload field
- 📁 Folder for browse button
- ⟳ Refresh symbol
- ✓ Checkmark for inject
- ✗ X-mark for unload

### Windows 11 Integration

#### Title Bar
- **Dark mode enabled** via DWM attributes
- **Rounded corners** on window (Windows 11 only)
- **Immersive dark mode** for system menu

#### Modern Spacing
- **15px margins** - Generous breathing room
- **10px spacing** - Between related elements
- **Consistent padding** - Professional layout

#### Smooth Interactions
- **Hover feedback** on all interactive elements
- **Visual states** clearly indicate clickability
- **No jarring transitions** - Everything flows

## 🚀 Functionality Highlights

### Smart Architecture Detection
The status column provides instant feedback:
- ✓ **Compatible** - Green light to proceed
- ✗ **Architecture mismatch** - Clear warning with solution

### Live Filtering
- Type in search box → Instant filter
- Change architecture dropdown → Instant update
- No lag, pure performance

### Intuitive Workflow
1. **Search** for your target process
2. **Click** to select
3. **Browse** for DLL or type name
4. **Click** Inject/Unload
5. **See** instant feedback

### Professional Touches
- **Status bar** with helpful messages
- **Keyboard shortcuts** (F5 refresh)
- **Error handling** with clear messages
- **Admin detection** - Automatic UAC elevation

## 💡 Design Philosophy

### Dark but Not Overwhelming
- Carefully chosen grays prevent eye strain
- High contrast text ensures readability
- Accent colors guide attention

### Modern but Familiar
- Windows 11 design language
- Familiar controls with modern styling
- No learning curve for Windows users

### Functional but Beautiful
- Every pixel serves a purpose
- Visual hierarchy guides workflow
- Form follows function

## 🎯 User Experience

### First Impression
When you launch the app, you're greeted with:
- Clean, dark interface
- Large, readable process list
- Clear action buttons
- Helpful status message

### During Use
- Instant visual feedback on hover
- Clear indication of selected items
- Status bar keeps you informed
- No confusion about next steps

### Success Feedback
- Message boxes for completion
- Updated status bar
- Clear success/error indicators

## 🔧 Technical Implementation

### Dark Mode Strategy
1. **Window Class** - Dark brush background
2. **DWM Integration** - System dark mode
3. **Custom Drawing** - Owner-draw controls
4. **Color Messages** - WM_CTLCOLOR* handlers
5. **ListView Theming** - Custom colors

### Performance
- **Double buffering** prevents flicker
- **Efficient redraws** only when needed
- **Smooth scrolling** in large lists
- **Fast filtering** with no lag

### Compatibility
- **Windows 10 1903+** - Rounded corners fallback
- **Windows 11** - Full feature set
- **High DPI** - Scales properly
- **Any theme** - Overrides system theme
