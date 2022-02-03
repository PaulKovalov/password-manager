import pytest
import shutil
import os
from pathlib import Path
from pm import init_storage, add_password, read_password, delete_password
from pm import USERNAME, STORAGE_PASSWORD, SITE_NAME

DEFAULT_USER = 'Vasiliy'
TEST_USER_1 = 'Vasya'


@pytest.fixture(autouse=True)
def run_before_and_after_tests():
    # This is where a test function is called.
    yield
    # Remove all created storages.
    shutil.rmtree(os.path.join('/home', DEFAULT_USER, '.pmp'))


def test_create_default_storage():
    test_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123',
    }
    init_storage(test_context)
    expected_file_path = os.path.join('/home', DEFAULT_USER, '.pmp',
                                      '81d1d68e2230674aa504d7fa2efaa8b893a128d3cc477385f26fa106380fb004.pst')
    file_exists = Path(expected_file_path).exists()
    assert file_exists, "Storage file was not created"
    with open(expected_file_path) as file:
        written_password = file.readline().rstrip()
        assert written_password == '4f42378d53b50a1c73d9a57f1550540ea0c3c0d5ceaf671394f2d200d4f95eafdcd6ff816c08e4c4364dc3ccc8bcf5a29cb003f05ff3089df5c852dc30d8fd8c'


def test_create_storage_for_user():
    test_context = {
        USERNAME: TEST_USER_1,
        STORAGE_PASSWORD: 'Qwerty123',
    }
    init_storage(test_context)
    expected_file_path = os.path.join('/home', DEFAULT_USER, '.pmp',
                                      '13e661b9c86dd74ddc9e1d971bed6e19348cd00a84a2e020a7b76f1f094706da.pst')
    file_exists = Path(expected_file_path).exists()
    assert file_exists, "Storage file was not created"
    with open(expected_file_path) as file:
        written_password = file.readline().rstrip()
        assert written_password == '4f42378d53b50a1c73d9a57f1550540ea0c3c0d5ceaf671394f2d200d4f95eafdcd6ff816c08e4c4364dc3ccc8bcf5a29cb003f05ff3089df5c852dc30d8fd8c'


def test_add_password_to_empty_storage(mocker):
    init_context = {
        USERNAME: TEST_USER_1,
        STORAGE_PASSWORD: 'Qwerty123'
    }
    init_storage(init_context)
    test_context = {
        USERNAME: TEST_USER_1,
        STORAGE_PASSWORD: 'Qwerty123',
        SITE_NAME: 'Vasya-site'
    }
    test_password = 'Qwerty124'
    # Mock input to return test password.
    mocker.patch('pm.prompt_password', return_value=(test_password, test_password))
    add_password(test_context)
    expected_file_path = os.path.join('/home', DEFAULT_USER, '.pmp',
                                      '13e661b9c86dd74ddc9e1d971bed6e19348cd00a84a2e020a7b76f1f094706da.pst')
    with open(expected_file_path) as file:
        # The first line is storage password.
        assert len(file.readlines()) == 3


def test_add_password_to_non_empty_storage(mocker):
    init_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123'
    }
    init_storage(init_context)
    test_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123',
        SITE_NAME: 'Vasya-site'
    }
    # Pre-fill storage.
    test_password = 'Qwerty124'
    # Mock input to return test password.
    mocker.patch('pm.prompt_password', return_value=(test_password, test_password))
    add_password(test_context)

    # Update test context.
    test_context[SITE_NAME] = 'Vasya-place'
    test_password = 'Qwerty125'
    mocker.patch('pm.prompt_password', return_value=(test_password, test_password))
    add_password(test_context)

    expected_file_path = os.path.join('/home', DEFAULT_USER, '.pmp',
                                      '81d1d68e2230674aa504d7fa2efaa8b893a128d3cc477385f26fa106380fb004.pst')
    with open(expected_file_path) as file:
        # The first line is storage password.
        # The next two are the first password.
        # The next two are the second password.
        assert len(file.readlines()) == 5


def test_read_password(mocker):
    init_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123'
    }
    init_storage(init_context)
    test_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123',
        SITE_NAME: 'Vasya-site'
    }
    # Pre-fill storage.
    want_password = 'Qwerty228'
    # Mock input to return test password.
    mocker.patch('pm.prompt_password', return_value=(want_password, want_password))
    add_password(test_context)
    err, got_password = read_password(test_context)
    assert err is None
    assert got_password == want_password


def test_delete_password(mocker):
    init_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123'
    }
    init_storage(init_context)
    test_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123',
        SITE_NAME: 'Vasya-site'
    }
    # Pre-fill storage.
    test_password = 'Qwerty229'
    # Mock input to return test password.
    mocker.patch('pm.prompt_password', return_value=(test_password, test_password))
    add_password(test_context)
    test_context[SITE_NAME] = 'Vasya-place'
    test_password = 'Qwerty230'
    mocker.patch('pm.prompt_password', return_value=(test_password, test_password))
    add_password(test_context)
    expected_file_path = os.path.join('/home', DEFAULT_USER, '.pmp',
                                      '81d1d68e2230674aa504d7fa2efaa8b893a128d3cc477385f26fa106380fb004.pst')
    with open(expected_file_path) as file:
        # 1 storage password and 2 passwords, each 2 lines.
        assert len(file.readlines()) == 5

    # After deletion there should be only 1 password left, meaning 3 lines.
    # Context must be the same as used in add_password call.
    delete_password(test_context)
    with open(expected_file_path) as file:
        # 1 storage password and 1 password consisting of 2 lines.
        assert len(file.readlines()) == 3


def test_add_existing_password(mocker):
    init_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123'
    }
    init_storage(init_context)
    test_context = {
        USERNAME: DEFAULT_USER,
        STORAGE_PASSWORD: 'Qwerty123',
        SITE_NAME: 'Vasya-site'
    }
    # Add the password for the first time
    test_password = 'Qwerty229'
    # Mock input to return test password.
    mocker.patch('pm.prompt_password', return_value=(test_password, test_password))
    add_password(test_context)

    expected_file_path = os.path.join('/home', DEFAULT_USER, '.pmp',
                                      '81d1d68e2230674aa504d7fa2efaa8b893a128d3cc477385f26fa106380fb004.pst')
    content_before = []
    # Expect 3 lines to be written to the file
    with open(expected_file_path) as file:
        # 1 storage password and 1 password, each 2 lines.
        content_before = file.readlines()
        assert len(content_before) == 3
    
    # Try to add password again, and expect to get same 3 lines
    add_password(test_context)
    content_after = []
    with open(expected_file_path) as file:
        # 1 storage password and 1 password, each 2 lines.
        content_after = file.readlines()
        assert len(content_after) == 3
    # Ensure content of the storage has not changed.
    for b, a in zip(content_before, content_after):
        assert b == a

