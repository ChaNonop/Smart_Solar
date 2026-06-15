# PowerShell script to start FastAPI backend with virtual environment setup

if (-not (Test-Path venv)) {
    Write-Host "Creating Python Virtual Environment..." -ForegroundColor Cyan
    python -m venv venv
}

Write-Host "Activating Virtual Environment..." -ForegroundColor Cyan
# Set execution policy for process if not already allowed, to avoid script run issues
$originalExecutionPolicy = Get-ExecutionPolicy
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process -Force

. .\venv\Scripts\Activate.ps1

Write-Host "Upgrading pip and installing requirements..." -ForegroundColor Cyan
python -m pip install --upgrade pip
pip install -r requirements.txt

Write-Host "Starting FastAPI backend server at http://localhost:8000" -ForegroundColor Green
Write-Host "Press Ctrl+C to stop." -ForegroundColor Yellow
uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
